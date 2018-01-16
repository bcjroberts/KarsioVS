//
// Created by BCJRo on 2018-01-10.
//

#include "Component.h"

Component::Component(ComponentType type) {
    myComponentType = type;
}

ComponentType Component::getComponentType() {
    return myComponentType;
}

int Component::getEventBitmask() {
	return 0;
}

void Component::processFrameUpdate() {
	// Intended on being overriden in subclass, but it is not required to be.
	printf("General component processing frame update");
}

Component::~Component() {

}