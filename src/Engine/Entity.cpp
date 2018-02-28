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

Component* Entity::getComponent(ComponentType type) {
    for (auto & myComponent : myComponents)
    {
        if (myComponent->getComponentType() == type)
        {
            return myComponent;
        }
    }
    return nullptr;
}

// Removes component based on unique identifier
void Entity::removeComponent(Component* c) {
    removeComponent(c->id);
}

void Entity::removeComponent(int cid) {
    for (int i = 0; i < myComponents.size(); ++i) {
        if (myComponents[i]->id == cid) {
            myComponents.erase(myComponents.begin() + i);
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

glm::vec3 Entity::getPosition() const {
	return position;
}

glm::vec3 Entity::getCoarsePosition() const {
	float x = position.x;
	float y = position.y;
	float z = position.z;
	float gridSize = 10; // larger size = larger grid
	x = (x >= 0) ? (x - fmod(x, gridSize)) / gridSize : (x + fmod(x, gridSize)) / gridSize;
	y = (y - fmod(y, gridSize)) / gridSize;
	z = (z >= 0) ? (z - fmod(z, gridSize)) / gridSize : (z + fmod(z, gridSize)) / gridSize;

	return glm::vec3(x, y, z);
}

glm::quat Entity::getRotation() const {
    return rotation;
}

glm::vec3 Entity::getForwardVector() const {
	glm::vec3 forwardVec(0, 0, 1);
	forwardVec = glm::rotate(rotation, forwardVec);
	return forwardVec;
}

glm::vec3 Entity::getUpVector() const {
    glm::vec3 upVec(0, 1, 0);
    upVec = glm::rotate(rotation, upVec);
    return upVec;
}

glm::vec3 Entity::getRightVector() const {
    glm::vec3 rightVec(1, 0, 0);
    rightVec = glm::rotate(rotation, rightVec);
    return rightVec;
}

glm::mat4 Entity::getMatrix() const {
    glm::mat4 myMatrix;
    myMatrix = glm::translate(myMatrix, position) * glm::toMat4(rotation) * glm::scale(myMatrix, scale);
    return myMatrix;
}

Entity::~Entity() = default;
