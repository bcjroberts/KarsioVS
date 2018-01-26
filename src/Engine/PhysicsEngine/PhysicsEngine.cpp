//
// Created by Sebastian on 12/23/2017.
//

#include "PhysicsEngine.h"
#include "PxPhysicsAPI.h"


#include "vehicle/PxVehicleUtil.h"
#include "VehicleCreation/SnippetVehicleConcurrency.h"
#include "VehicleCreation/SnippetVehicleSceneQuery.h"
#include "VehicleCreation/SnippetVehicleTireFriction.h"
#include "VehicleCreation/SnippetVehicleFilterShader.h"
#include "VehicleCreation/SnippetVehicleCreate.h"


physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;

physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;

physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;

physx::PxCooking* gCooking = NULL;

physx::PxMaterial* gMaterial = NULL;

physx::PxPvd* gPvd = NULL;

physx::PxRigidStatic* gGroundPlane = NULL;

snippetvehicle::VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
physx::PxBatchQuery*			gBatchQuery = NULL;
physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PhysicsEngine::PhysicsEngine() {

}

enum {
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};

void setupDrivableSurface(physx::PxFilterData& filterData) {
	filterData.word3 = static_cast<physx::PxU32>(DRIVABLE_SURFACE);
}

physx::PxRigidStatic* createDrivablePlane(const physx::PxFilterData& simFilterData, physx::PxMaterial* material, physx::PxPhysics* physics) {
	//Add a plane to the scene.
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *material);

	//Get the plane shape so we can set query and simulation filter data.
	physx::PxShape* shapes[1];
	groundPlane->getShapes(shapes, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	physx::PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	shapes[0]->setSimulationFilterData(simFilterData);

	return groundPlane;
}

void PhysicsEngine::initPhysics()
{
    gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

    physx::PxU32 numWorkers = 1;
    gDispatcher = physx::PxDefaultCpuDispatcherCreate(numWorkers);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);
    physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    gMaterial = gPhysics->createMaterial(0.0f, 0.0f, 0.2f);

    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));

    /////////////////////////////////////// Init vehicle stuff, including currently the ground plane though this should be moved

    PxInitVehicleSDK(*gPhysics);
    PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(5, PX_MAX_NB_WHEELS, 1, 1, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(gMaterial);

    //Create a plane to drive on. TODO: Move this elswhere, should not happen here!
    physx::PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
    gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
    gScene->addActor(*gGroundPlane);
}

snippetvehicle::VehicleDesc initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const physx::PxF32 chassisMass = 1500.0f;
    const physx::PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
    const physx::PxVec3 chassisMOI
        ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    const physx::PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const physx::PxF32 wheelMass = 20.0f;
    const physx::PxF32 wheelRadius = 0.5f;
    const physx::PxF32 wheelWidth = 0.4f;
    const physx::PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
    const physx::PxF32 nbWheels = 4;

    snippetvehicle::VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = gMaterial;
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = gMaterial;
    vehicleDesc.wheelSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

vehicleData* PhysicsEngine::createVehicle(physx::PxVec3 startPos) {

    // Create the vehicle
    physx::PxVehicleDrive4W* gVehicle4W = NULL;
    snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc();
    gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);

    // Sets its position and add to the physics simulation
    physx::PxTransform startTransform(startPos + physx::PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), physx::PxQuat(physx::PxIdentity));
    gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
    gScene->addActor(*gVehicle4W->getRigidDynamicActor());

    // Set it to resting, let auto gears happen as we don't want gear shifting
    gVehicle4W->setToRestState();
    gVehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
    gVehicle4W->mDriveDynData.setUseAutoGears(true);

    // Create the vehicleData object to store the data, push to vector and return
    vehicleData* vd = new vehicleData();
    vd->myVehicle = gVehicle4W;
	allVehicleData.push_back(vd);
    return vd;
}

// When using a digital controller, how quickly the value reaches 1.
physx::PxVehicleKeySmoothingData gKeySmoothingData =
{
    {
        6.0f,	//rise rate eANALOG_INPUT_ACCEL
        6.0f,	//rise rate eANALOG_INPUT_BRAKE		
        6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
        2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
        2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
    },
    {
        10.0f,	//fall rate eANALOG_INPUT_ACCEL
        10.0f,	//fall rate eANALOG_INPUT_BRAKE		
        10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
        5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
        5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
    }
};
physx::PxVehiclePadSmoothingData gPadSmoothingData =
{
    {
        6.0f,	//rise rate eANALOG_INPUT_ACCEL
        6.0f,	//rise rate eANALOG_INPUT_BRAKE		
    6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
    2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
    2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
    },
    {
        10.0f,	//fall rate eANALOG_INPUT_ACCEL
        10.0f,	//fall rate eANALOG_INPUT_BRAKE		
    10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
    5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
    5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
    }
};
physx::PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
    0.0f,		0.75f,
    5.0f,		0.75f,
    30.0f,		0.125f,
    120.0f,		0.1f,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32
};
physx::PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);


// Tell physX to simulate the physics, takes time as a float in seconds.
void PhysicsEngine::simulateTimeInSeconds(float timeInSeconds) const {

	const int numberOfVehicles = allVehicleData.size();

    // Set the vehicle input information and perform wheel raycasts
	physx::PxVehicleWheels* vehicles[PX_MAX_NB_VEHICLES];
	for (int i = 0; i < numberOfVehicles; i++)
	{
		// Set the vehicle moving
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, allVehicleData[i]->myInput, timeInSeconds, allVehicleData[i]->isInAir, *allVehicleData[i]->myVehicle);
		//PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timeInSeconds, gIsVehicleInAir, *gVehicle4W);

		vehicles[i] = allVehicleData[i]->myVehicle;
	}
    physx::PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const physx::PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(gBatchQuery, numberOfVehicles, vehicles, raycastResultsSize, raycastResults);

    // Update each vehicle based on the wheel queries
    const physx::PxVec3 grav = gScene->getGravity();
	physx::PxVehicleWheelQueryResult vehicleQueryResults[PX_MAX_NB_VEHICLES];
	for (int i = 0; i < numberOfVehicles; i++)
	{
		physx::PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		vehicleQueryResults[i] = { wheelQueryResults, allVehicleData[i]->myVehicle->mWheelsSimData.getNbWheels() };
	}
    PxVehicleUpdates(timeInSeconds, grav, *gFrictionPairs, numberOfVehicles, vehicles, vehicleQueryResults);

    //Check each vehicle to see if it is in the air
    for (int i = 0; i < numberOfVehicles; i++) {
        allVehicleData[i]->isInAir = allVehicleData[i]->myVehicle->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[i]);
    }

    gScene->simulate(timeInSeconds);
    gScene->fetchResults(true);
}

PhysicsEngine::~PhysicsEngine() = default;
