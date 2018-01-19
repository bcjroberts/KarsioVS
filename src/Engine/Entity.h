//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_ENTITY_H
#define PROJECTKARSIO_ENTITY_H

#include <glm/glm.hpp>
#include "vector"
#include <glm/gtc/quaternion.hpp>
#include "Event.h"

class Component;

class Entity {
private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    std::vector<Component*> myComponents;
public:
    int id;
    Entity(glm::vec3 nposition, glm::quat nrotation, glm::vec3 nscale);
    void addComponent(Component* c);
	void removeComponent(Component* c);
    void removeComponent(int cid);
    ~Entity();
};


#endif //PROJECTKARSIO_ENTITY_H
