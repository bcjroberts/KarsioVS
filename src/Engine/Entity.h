//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_ENTITY_H
#define PROJECTKARSIO_ENTITY_H

#include <glm/glm.hpp>
#include "Component.h"
#include "vector"

class Entity {
private:
    int id;
    glm::vec3 position;
    glm::vec4 rotation;
    float scale;
    std::vector<Component> myComponents;
public:
    Entity(glm::vec3 nposition, glm::vec4 nrotation, float nscale);
    void addComponent(Component c);
    void removeComponent(Component c);
    ~Entity();
};


#endif //PROJECTKARSIO_ENTITY_H
