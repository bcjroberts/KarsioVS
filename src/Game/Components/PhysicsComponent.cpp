#include "PhysicsComponent.h"



PhysicsComponent::PhysicsComponent(physx::PxRigidActor* nactor) : Component(PHYSICS) {
	myActor = nactor;
}

physx::PxRigidBody* PhysicsComponent::getRigidBody() const {
    return static_cast<physx::PxRigidBody*>(myActor);
}

PhysicsComponent::~PhysicsComponent() = default;

