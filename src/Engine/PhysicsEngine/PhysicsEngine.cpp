//
// Created by Sebastian on 12/23/2017.
//

#include "PhysicsEngine.h"
#include "PxPhysicsAPI.h"

physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;

physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;

physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;

physx::PxMaterial* gMaterial = NULL;

physx::PxPvd* gPvd = NULL;

PhysicsEngine::PhysicsEngine() {

}

void PhysicsEngine::initPhysics() {
    gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

    printf("Creating physX...");

    gPvd = PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), false, NULL);

    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
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
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // Setup the ground plane now, this will always remain in the scene.
    // THIS SHOULD BE A PHYSICS COMPONENT OF AN ENTITY with a MESH!!! ***TODO***
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);
    printf("PhysX created!!");
}

// Tell physX to simulate the physics, takes time as a float in seconds.
void PhysicsEngine::simulateTimeInSeconds(float timeInSeconds) {
    gScene->simulate(timeInSeconds);
    gScene->fetchResults(true);
}

PhysicsEngine::~PhysicsEngine()
{

}
