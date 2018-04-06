//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_COMPONENT_H
#define PROJECTKARSIO_COMPONENT_H

#include <stdio.h>

enum ComponentType {RENDERER, PHYSICS, DRIVE, HEALTH, AI, CONTROLLABLE, UPGRADE, WEAPON, PROJECTILE, STATIC_LIGHT, FLOATING_TEXT };
class Entity;

class Component {
private:
    ComponentType myComponentType;
public:
    Entity* owner;
    int id;
    Component(ComponentType type);
	ComponentType getComponentType();
	virtual ~Component();
};


#endif //PROJECTKARSIO_COMPONENT_H
