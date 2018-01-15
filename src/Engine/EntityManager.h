#pragma once
#include "Entity.h"
#include "vector"

class EntityManager
{
public:
    EntityManager();
    void createEntity();
    void destoryEntity(int id);
    ~EntityManager();
private:
    std::vector<Entity> entities;
};

