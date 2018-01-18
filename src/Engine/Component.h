//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_COMPONENT_H
#define PROJECTKARSIO_COMPONENT_H

#include <stdio.h>

enum ComponentType {RENDERER, PHYSICS};

class Component {
private:
    ComponentType myComponentType;
public:
    Component(ComponentType type);
	ComponentType getComponentType();
    ~Component();
};


#endif //PROJECTKARSIO_COMPONENT_H
