//
// Created by BCJRo on 2018-01-10.
//

#include "Entity.h"
#include "Component.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.inl>

Entity::Entity(glm::vec3 nposition, glm::quat nrotation, glm::vec3 nscale) {
    static int nextId = 0;
    id = nextId++;
    position = nposition;
    rotation = nrotation;
	scale = nscale;
}

void Entity::addComponent(Component* c) {
    myComponents.push_back(c);
    c->owner = this;
}

// Removes component based on unique identifier
void Entity::removeComponent(Component* c) {
    removeComponent(c->id);
}

void Entity::removeComponent(int cid) {
    for (int i = 0; i < myComponents.size(); ++i) {
        if (myComponents[i]->id == cid) {
            Component* c = myComponents[i]; // keep for cleanup purpose
            myComponents.erase(myComponents.begin() + i);
            delete c;
            break;
        }
    }
}

void Entity::updatePosition(glm::vec3 nposition) {
    position = nposition;
}

void Entity::updateRotation(glm::quat nrotation) {
    rotation = nrotation;
}


glm::mat4 Entity::getMatrix() const {
    glm::mat4 myMatrix;
    myMatrix = glm::translate(myMatrix, position) * glm::toMat4(rotation) * glm::scale(myMatrix, scale);
    return myMatrix;
}

Entity::~Entity() = default;
