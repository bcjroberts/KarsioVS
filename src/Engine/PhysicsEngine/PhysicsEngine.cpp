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
#include <glm/detail/type_vec3.hpp>
#include "CollisionProcessor.h"

// Initialize the Physics Manager global pointer
PhysicsEngine *PhysicsEngine::globalInstance = nullptr;

physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;

physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;

physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;

physx::PxCooking* gCooking = NULL;

physx::PxMaterial* gMaterial = NULL;

physx::PxPvd* gPvd = NULL;

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

// TODO: This method needs to be modified to fit out reporting needs!
physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
    
    //if( physx::PxFilterObjectIsTrigger( attributes0 ) || physx::PxFilterObjectIsTrigger( attributes1 ) )
    // Check if either is a trigger, though we may not be using triggers
    

	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
		| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
		//| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return physx::PxFilterFlag::eDEFAULT;
}

CollisionProcessor colproc;

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
    sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &colproc;

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
    gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(PX_MAX_NB_VEHICLES, PX_MAX_NB_WHEELS, 1, 15, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(gMaterial);
}

snippetvehicle::VehicleDesc initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const physx::PxF32 chassisMass = 1500.0f;
    const physx::PxVec3 chassisDims(3.0f, 2.0f, 5.0f);
    const physx::PxVec3 chassisMOI
        ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    const physx::PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.8f, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const physx::PxF32 wheelMass = 20.0f;
    const physx::PxF32 wheelRadius = 0.8f;
    const physx::PxF32 wheelWidth = 0.4f;
    const physx::PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;

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
    vehicleDesc.numWheels = 4;
    vehicleDesc.wheelMaterial = gMaterial;
    vehicleDesc.wheelSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

bool scaleUpVehicle = false;

physx::PxVec3 PhysicsEngine::toPxVec3(glm::vec3 from) {
	return physx::PxVec3(from.x, from.y, from.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// START: Section where physics objects are created

physx::PxRigidActor* PhysicsEngine::createPhysicsPlane() {

	const physx::PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
    physx::PxRigidStatic* plane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);

    //Get the plane shape so we can set query and simulation filter data.
    physx::PxShape* shapes[1];
    plane->getShapes(shapes, 1);

    //Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
    physx::PxFilterData qryFilterData;
    setupDrivableSurface(qryFilterData);
    shapes[0]->setQueryFilterData(qryFilterData);

    //Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
    shapes[0]->setSimulationFilterData(groundPlaneSimFilterData);

	gScene->addActor(*plane);
	return plane;
}

physx::PxRigidActor* PhysicsEngine::createPhysicsBox(physx::PxVec3 pos, physx::PxVec3 scale) {
	
	//const physx::PxFilterData boxSimFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
    const physx::PxFilterData boxSimFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	physx::PxShape* boxShape = gPhysics->createShape(physx::PxBoxGeometry(scale),*gMaterial);
    boxShape->setSimulationFilterData(boxSimFilterData);

	// If you want to set the box to be a trigger, uncomment the following
	//boxShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	//boxShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

	physx::PxRigidActor* box = physx::PxCreateStatic(*gPhysics,physx::PxTransform(pos),*boxShape);
	gScene->addActor(*box);
	return box;
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

    // Test of scaling vehicles up.
    if (scaleUpVehicle)
    {
        scaleUpVehicle = false;
        snippetvehicle::customizeVehicleToLengthScale(2.0f, gVehicle4W->getRigidDynamicActor(), &gVehicle4W->mWheelsSimData, &gVehicle4W->mDriveSimData);
    }

    // Create the vehicleData object to store the data, push to vector and return
    vehicleData* vd = new vehicleData();
    vd->myVehicle = gVehicle4W;
	allVehicleData.push_back(vd);
    return vd;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// END: Section where physics objects are created

PhysicsEngine* PhysicsEngine::getInstance()
{
    if (!globalInstance) {
        globalInstance = new PhysicsEngine;
    }
    return globalInstance;
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
        // Put the vehicle in the correct gear:
        if (allVehicleData[i]->myInput.getDigitalBrake()) {
            // if we are breaking, set accelration to true and ensure we are in the reverse gear
            allVehicleData[i]->myInput.setDigitalAccel(true);
            allVehicleData[i]->myInput.setDigitalBrake(false);
            allVehicleData[i]->myVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
        } else {
            // otherwise ensure we are in first gear
            allVehicleData[i]->myVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
        }

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
