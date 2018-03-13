//
// Created by Sebastian on 12/23/2017.
//

#pragma once
#ifndef RENDERENGINE_PHYSICSENGINE_H
#define RENDERENGINE_PHYSICSENGINE_H
#include <PxActor.h>
#include <vehicle/PxVehicleDrive4W.h>
#include <vector>
#include <vehicle/PxVehicleUtilControl.h>
#include <glm/detail/type_vec3.hpp>
#include <geometry/PxGeometryQuery.h>
#include "../../Game/Components/DriveComponent.h"

struct vehicleData {
    bool isInAir = false;
    physx::PxVehicleDrive4W* myVehicle;
	vehicleInput myInput;
    physx::PxVehicleDrive4WRawInputData myPxInput;
};

struct RaycastData {
    bool hit;
};

class PhysicsEngine {
private:
	std::vector<vehicleData*> allVehicleData;
    static PhysicsEngine* globalInstance;
public:
    PhysicsEngine();
    void simulateTimeInSeconds(float timeInSeconds) const;
    void initPhysics(); // This method must be called before anything else physics related happens
	physx::PxRigidActor* createPhysicsPlane();
	physx::PxRigidActor* createPhysicsBox(physx::PxVec3 pos, physx::PxVec3 scale, physx::PxQuat rotation = physx::PxIdentity) const;
    physx::PxRigidActor* createCrystalBoxCollider(physx::PxVec3 pos, physx::PxVec3 scale) const;
    vehicleData* createVehicle(physx::PxVec3 startpos);
    bool fireRaycast(physx::PxRaycastBuffer* dataToFill, physx::PxVec3 origin, physx::PxVec3 dir, float distance);
    static PhysicsEngine* getInstance();
	static physx::PxVec3 PhysicsEngine::toPxVec3(glm::vec3 from);
    static glm::vec3 PhysicsEngine::toglmVec3(physx::PxVec3 from);
    void modifyVehicleScale(float scale, physx::PxRigidDynamic* rigid, physx::PxVehicleDrive4W* vehicle);
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
