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

struct vehicleData {
    bool isInAir = false;
    physx::PxVehicleDrive4W* myVehicle;
    physx::PxVehicleDrive4WRawInputData myInput;
};

class PhysicsEngine {
private:
	std::vector<vehicleData*> allVehicleData;
public:
    PhysicsEngine();
    void simulateTimeInSeconds(float timeInSeconds) const;
    void initPhysics(); // This method must be called before anything else physics related happens
    vehicleData* createVehicle(physx::PxVec3 startpos);
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
