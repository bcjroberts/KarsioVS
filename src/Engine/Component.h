//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_COMPONENT_H
#define PROJECTKARSIO_COMPONENT_H

enum ComponentType {RENDERER, PHYSICS};

class Component {
private:
    bool initialized = false;
    ComponentType myComponentType;
public:
    Component(ComponentType type);
    ComponentType getComponentType();
    int getEventBitmask();
    ~Component();
};


#endif //PROJECTKARSIO_COMPONENT_H
