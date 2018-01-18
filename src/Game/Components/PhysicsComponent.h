#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "PxPhysicsAPI.h"

class PhysicsComponent: public Component
{
public:
	physx::PxActor* myActor;
	PhysicsComponent();
	~PhysicsComponent();
};

