//
// Created by Sebastian on 12/23/2017.
//

#ifndef RENDERENGINE_PHYSICSENGINE_H
#define RENDERENGINE_PHYSICSENGINE_H
#include <PxActor.h>

class PhysicsEngine {

public:
    PhysicsEngine();
    void simulateTimeInSeconds(float timeInSeconds);
    void initPhysics(); // This method must be called before anything else physics related happens
	physx::PxActor* createPhysicsActor();
    ~PhysicsEngine();
};


#endif //RENDERENGINE_PHYSICSENGINE_H
