#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "PxPhysicsAPI.h"

class PhysicsComponent: public Component
{
public:
	physx::PxRigidActor* myActor;
	PhysicsComponent(physx::PxRigidActor* nactor);
    physx::PxRigidBody* getRigidBody() const;
	~PhysicsComponent();
};

