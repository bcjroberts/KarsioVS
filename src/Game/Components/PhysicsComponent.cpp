#include "PhysicsComponent.h"



PhysicsComponent::PhysicsComponent() : Component(PHYSICS) {
	myActor = nullptr;
}


PhysicsComponent::~PhysicsComponent() = default;

