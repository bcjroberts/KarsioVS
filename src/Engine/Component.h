//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_COMPONENT_H
#define PROJECTKARSIO_COMPONENT_H

#include <stdio.h>
#include "Entity.h"

enum ComponentType {RENDERER, PHYSICS, DRIVE};

class Component {
private:
    ComponentType myComponentType;
public:
    Entity* owner;
    int id;
    Component(ComponentType type);
	ComponentType getComponentType();
    ~Component();
};


#endif //PROJECTKARSIO_COMPONENT_H
