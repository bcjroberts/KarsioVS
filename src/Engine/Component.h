//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_COMPONENT_H
#define PROJECTKARSIO_COMPONENT_H

enum ComponentType {Type1, Type2};

class Component {
private:
    ComponentType myComponentType;
public:
    Component(ComponentType type);
    ComponentType getComponentType();
    ~Component();

};


#endif //PROJECTKARSIO_COMPONENT_H
