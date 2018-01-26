//
// Created by Sebastian on 12/23/2017.
//

#pragma once
#ifndef RENDERENGINE_PHYSICSENGINE_H
#define RENDERENGINE_PHYSICSENGINE_H
#include <PxActor.h>
#include <vehicle/PxVehicleDrive4W.h>
#include <vector>
#include "../ComponentManager.h"

class PhysicsEngine {
private:
	std::vector<physx::PxVehicleDrive4W*> allVehicles;
public:
    std::vector<DriveComponent*>* driveComponentList;
    void bindDriveList(std::vector<DriveComponent*> * driveList);
    PhysicsEngine();
    void simulateTimeInSeconds(float timeInSeconds) const;
    void initPhysics(); // This method must be called before anything else physics related happens
    physx::PxVehicleDrive4W* createVehicle(physx::PxVec3 startpos);
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
