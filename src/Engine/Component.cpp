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

Component::~Component() = default;