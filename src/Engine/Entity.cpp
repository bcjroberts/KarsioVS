//
// Created by BCJRo on 2018-01-10.
//

#include "Entity.h"

Entity::Entity(glm::vec3 nposition, glm::vec4 nrotation, float nscale) {
    static int nextId = 0;
    id = nextId++;
    position = nposition;
    rotation = nrotation;
    scale = nscale;
}

void Entity::addComponent(Component c) {
    myComponents.push_back(c);
}

// Currently only removes components by looking at its type.
// This is NOT a good solution.
void Entity::removeComponent(Component c) {
    for (int i = 0; i < myComponents.size(); ++i) {
        if (myComponents[i].getComponentType() == c.getComponentType()) {
            myComponents.erase(myComponents.begin() + i);
            break;
        }
    }
}

Entity::~Entity() {

}