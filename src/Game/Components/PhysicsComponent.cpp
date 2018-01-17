#include "PhysicsComponent.h"



PhysicsComponent::PhysicsComponent() : Component(PHYSICS) {
	eventBitmask = 0;
}


PhysicsComponent::~PhysicsComponent() = default;

int PhysicsComponent::getEventBitmask() {
	return eventBitmask;
}

void PhysicsComponent::processFrameUpdate() {

}
