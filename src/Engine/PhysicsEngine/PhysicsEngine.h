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

struct vehicleData {
    bool isInAir = false;
    physx::PxVehicleDrive4W* myVehicle;
    physx::PxVehicleDrive4WRawInputData myInput;
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
	physx::PxRigidActor* createPhysicsBox(physx::PxVec3 pos, physx::PxVec3 scale) const;
    physx::PxRigidActor* createCrystalBoxCollider(physx::PxVec3 pos, physx::PxVec3 scale) const;
    vehicleData* createVehicle(physx::PxVec3 startpos);
    static PhysicsEngine* getInstance();
	static physx::PxVec3 PhysicsEngine::toPxVec3(glm::vec3 from);
    static glm::vec3 PhysicsEngine::toglmVec3(physx::PxVec3 from);
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
