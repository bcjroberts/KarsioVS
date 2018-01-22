//
// Created by Sebastian on 12/23/2017.
//

#ifndef RENDERENGINE_PHYSICSENGINE_H
#define RENDERENGINE_PHYSICSENGINE_H
#include <PxActor.h>
#include <vehicle/PxVehicleDrive4W.h>

class PhysicsEngine {

public:
    PhysicsEngine();
    void simulateTimeInSeconds(float timeInSeconds, physx::PxVehicleDrive4W* gVehicle4W) const;
    void initPhysics(); // This method must be called before anything else physics related happens
    physx::PxVehicleDrive4W* createVehicle();
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
