#pragma once
#include "Entity.h"
#include "vector"

class EntityManager
{
public:
    EntityManager();
    Entity* createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 nscale);
    void destroyEntity(int id);
    ~EntityManager();
private:
    std::vector<Entity*> entities;
};

