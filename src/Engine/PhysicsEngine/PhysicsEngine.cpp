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
#include "../../game/components/PhysicsComponent.h"
#include "../EntityManager.h"
#include "VehicleConfigParser.h"
#include "../../Game/Components/UpgradeComponent.h"

// Initialize the Physics Manager global pointer
PhysicsEngine *PhysicsEngine::globalInstance = nullptr;

physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;

physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;

physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;

physx::PxCooking* gCooking = NULL;

physx::PxMaterial* baseMaterial = NULL;
physx::PxMaterial* boxMaterial = NULL;
physx::PxMaterial* vehicleBoxMat = NULL;
physx::PxMaterial* drillMat = NULL;

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
    
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

    //if( physx::PxFilterObjectIsTrigger( attributes0 ) || physx::PxFilterObjectIsTrigger( attributes1 ) )
    // Check if either is a trigger, though we may not be using triggers

	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;

	// If neither are equal to 0, then we need to process some things
    if ((filterData0.word2 == snippetvehicle::COLLISION_FLAG_DRILL && filterData1.word3 == snippetvehicle::COLLISION_FLAG_CRYSTAL) || 
		(filterData1.word2 == snippetvehicle::COLLISION_FLAG_DRILL && filterData0.word3 == snippetvehicle::COLLISION_FLAG_CRYSTAL)) {
		// Yay this works!
		pairFlags = pairFlags
			//| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
			//| physx::PxPairFlag::ePRE_SOLVER_VELOCITY
			//| physx::PxPairFlag::ePOST_SOLVER_VELOCITY
			//| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS // Not sure if we want this one.
			| physx::PxPairFlag::eMODIFY_CONTACTS;
			//| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
    }
	
	if ((filterData0.word2 == snippetvehicle::COLLISION_FLAG_DRILL && filterData1.word3 == snippetvehicle::COLLISION_FLAG_CAR) ||
		(filterData1.word2 == snippetvehicle::COLLISION_FLAG_DRILL && filterData0.word3 == snippetvehicle::COLLISION_FLAG_CAR)) {
		// Yay this works!
		pairFlags = pairFlags
			| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
			| physx::PxPairFlag::ePRE_SOLVER_VELOCITY
			| physx::PxPairFlag::ePOST_SOLVER_VELOCITY;
			//| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS // Not sure if we want this one.
			//| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	}
	return physx::PxFilterFlag::eDEFAULT;
}

CollisionProcessor colproc;

void PhysicsEngine::bindAudioObservable(AudioObservable* anEventList) {
    audioEvents = anEventList;
    colproc.bindAudioObservable(anEventList);
}

void PhysicsEngine::initPhysics()
{
    gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -40.f, 0.0f);

    physx::PxU32 numWorkers = 1;
    gDispatcher = physx::PxDefaultCpuDispatcherCreate(numWorkers);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = contactReportFilterShader;
    sceneDesc.contactModifyCallback = &colproc;
	sceneDesc.simulationEventCallback = &colproc;

    gScene = gPhysics->createScene(sceneDesc);
    physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    baseMaterial = gPhysics->createMaterial(0.1f, 0.5f, 0.2f);
    vehicleBoxMat = gPhysics->createMaterial(0.f, 0.25f, 0.6f);
    drillMat = gPhysics->createMaterial(0.f, 0.25f, 0.1f);
    boxMaterial = gPhysics->createMaterial(0.f, 0.f, 0.2f);

    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));

    /////////////////////////////////////// Init vehicle stuff, including currently the ground plane though this should be moved

    PxInitVehicleSDK(*gPhysics);
    PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(PX_MAX_NB_VEHICLES, PX_MAX_NB_WHEELS, 1, 15, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(baseMaterial);
}

snippetvehicle::VehicleDesc initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const physx::PxF32 chassisMass = 1500.0f;
    const physx::PxVec3 chassisDims(3.0f, 2.5f, 5.0f);
    const physx::PxVec3 chassisMOI
        ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);

    // Centred on the x axis. This MUST be 0.
    // Y is set to be slightly lower to the ground than normal to prevent tipping.
    // Z is set to be centred, but can be adjusted slightly.
    const physx::PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f-0.5f, 0.25f);

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
    vehicleDesc.chassisMaterial = vehicleBoxMat;
    vehicleDesc.drillMaterial = drillMat;
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, snippetvehicle::COLLISION_FLAG_CAR);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = 4;
    vehicleDesc.wheelMaterial = baseMaterial;
    vehicleDesc.wheelSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, snippetvehicle::COLLISION_FLAG_CAR);

    return vehicleDesc;
}

bool scaleUpVehicle = false;

physx::PxVec3 PhysicsEngine::toPxVec3(glm::vec3 from) {
	return physx::PxVec3(from.x, from.y, from.z);
}

glm::vec3 PhysicsEngine::toglmVec3(physx::PxVec3 from) {
    return glm::vec3(from.x, from.y, from.z);
}

void PhysicsEngine::modifyVehicleScale(float scale, physx::PxRigidDynamic* rigid, physx::PxVehicleDrive4W* vehicle) {
    
    // First update the size of the vehicle
    snippetvehicle::customizeVehicleToLengthScale(scale, rigid, &vehicle->mWheelsSimData, &vehicle->mDriveSimData);

    int chassisLevel = static_cast<UpgradeComponent*>(static_cast<Entity*>(rigid->userData)->getComponent(UPGRADE))->getChassisLevel();
    switch(chassisLevel) {
        case 1:
            chassisType = "chassis1";
        case 2:
            chassisType = "chassis2";
        case 3:
            chassisType = "chassis3";
        default:
            chassisType = "chassis1";
        break;
    }
    // Then update its torque values
    VehicleConfigParser::getInstance()->parseConfigFile(chassisType);
    VehicleConfigParser::getInstance()->applyConfigToVehicle(vehicle);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// START: Section where physics objects are created

physx::PxRigidActor* PhysicsEngine::createPhysicsPlane() {

	const physx::PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
    physx::PxRigidStatic* plane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *baseMaterial);

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

physx::PxRigidActor* PhysicsEngine::createPhysicsBox(physx::PxVec3 pos, physx::PxVec3 scale, physx::PxQuat rotation) const {
	
	const physx::PxFilterData boxSimFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	physx::PxShape* boxShape = gPhysics->createShape(physx::PxBoxGeometry(scale),*boxMaterial);
    boxShape->setSimulationFilterData(boxSimFilterData);
    boxShape->setLocalPose(physx::PxTransform(rotation));

	physx::PxRigidActor* box = physx::PxCreateStatic(*gPhysics,physx::PxTransform(pos),*boxShape);
	gScene->addActor(*box);
	return box;
}

physx::PxRigidActor* PhysicsEngine::createCrystalBoxCollider(physx::PxVec3 pos, physx::PxVec3 scale) const {

    const physx::PxFilterData crystalSimFilterData(snippetvehicle::COLLISION_FLAG_OBSTACLE, snippetvehicle::COLLISION_FLAG_OBSTACLE_AGAINST, 0, snippetvehicle::COLLISION_FLAG_CRYSTAL);
    physx::PxShape* boxShape = gPhysics->createShape(physx::PxBoxGeometry(scale), *boxMaterial);
    boxShape->setSimulationFilterData(crystalSimFilterData);

    physx::PxRigidActor* box = physx::PxCreateStatic(*gPhysics, physx::PxTransform(pos), *boxShape);
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

    // Now lets read the car config file and update the values from there
    chassisType = "chassis1";
    VehicleConfigParser::getInstance()->parseConfigFile(chassisType);
    VehicleConfigParser::getInstance()->applyConfigToVehicle(gVehicle4W);

    // Create the vehicleData object to store the data, push to vector and return
    vehicleData* vd = new vehicleData();
    vd->myVehicle = gVehicle4W;
	allVehicleData.push_back(vd);
    return vd;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// END: Section where physics objects are created

bool PhysicsEngine::fireRaycast(physx::PxRaycastBuffer* dataToFill, physx::PxVec3 origin, physx::PxVec3 dir, float distance) {
    return gScene->raycast(origin, dir, distance, *dataToFill);
}

void PhysicsEngine::removePhysicsActor(physx::PxActor* actor) {
    gScene->removeActor(*actor);
}

void PhysicsEngine::removeVehicleData(physx::PxVehicleDrive4W* vehicleDrive) {
    for (int i = 0; i < allVehicleData.size(); ++i) {
        if (allVehicleData[i]->myVehicle == vehicleDrive) {
            vehicleData* temp = allVehicleData[i];
            allVehicleData.erase(allVehicleData.begin() + i);
            delete temp;
            break;
        }
    }
}

PhysicsEngine* PhysicsEngine::getInstance()
{
    if (!globalInstance) {
        globalInstance = new PhysicsEngine;
    }
    return globalInstance;
}

physx::PxVehiclePadSmoothingData gPadSmoothingData =
{
    {
        12.0f,	//rise rate eANALOG_INPUT_ACCEL
        12.0f,	//rise rate eANALOG_INPUT_BRAKE		
        12.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
        2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
        2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
    },
    {
        20.0f,	//fall rate eANALOG_INPUT_ACCEL
        20.0f,	//fall rate eANALOG_INPUT_BRAKE		
        20.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
        5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
        5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
    }
};
physx::PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
    0.0f,		1.f,
    15.0f,		1.0f,
    30.0f,		0.95f,//0.125f
    60.0f,		0.9f,//0.1f
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
		// Set the Pxinput to match the regular input
		allVehicleData[i]->myPxInput.setAnalogSteer(allVehicleData[i]->myInput.steering);
		allVehicleData[i]->myPxInput.setAnalogHandbrake(allVehicleData[i]->myInput.handbrake);

		if (allVehicleData[i]->myInput.reverse > 0.f) {
			allVehicleData[i]->myPxInput.setAnalogAccel(allVehicleData[i]->myInput.reverse);
			if (allVehicleData[i]->myVehicle->mDriveDynData.getCurrentGear() != physx::PxVehicleGearsData::eREVERSE) {
				allVehicleData[i]->myVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
			}
		} else {
			allVehicleData[i]->myPxInput.setAnalogAccel(allVehicleData[i]->myInput.accel);
			allVehicleData[i]->myVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		}

		// Set the vehicle moving
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, allVehicleData[i]->myPxInput, timeInSeconds, allVehicleData[i]->isInAir, *allVehicleData[i]->myVehicle);

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
	
    // Remove all of the destroyed physics stuff from the scene
    for (int i = 0; i < colproc.destroyedEntities.size(); ++i) {
        EntityManager::getInstance()->destroyEntity(colproc.destroyedEntities[i]->id);
    }
    // Clear the array so items are not removed twice
    colproc.destroyedEntities.clear();
	
}

PhysicsEngine::~PhysicsEngine() = default;
