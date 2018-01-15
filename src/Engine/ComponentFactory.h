#pragma once

#include "Component.h"

class ComponentFactory 
{
private:
public:
    ComponentFactory();
    ~ComponentFactory();
    Component * createComponent(ComponentType type);
};

