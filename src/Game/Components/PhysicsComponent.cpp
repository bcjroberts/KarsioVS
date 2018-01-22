#include "PhysicsComponent.h"



PhysicsComponent::PhysicsComponent(physx::PxRigidActor* nactor) : Component(PHYSICS) {
	myActor = nactor;
}

PhysicsComponent::~PhysicsComponent() = default;

