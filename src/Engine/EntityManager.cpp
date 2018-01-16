

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>

EntityManager::EntityManager() {

}

EntityManager::~EntityManager() {

}

// Returns a reference to the created entity
Entity EntityManager::createEntity(glm::vec3 nposition, glm::quat nrotation, glm::vec3 nscale) {
    Entity newEntity(nposition, nrotation, nscale);
    entities.push_back(&newEntity);
    return newEntity;
}

void EntityManager::destroyEntity(int id) {
    for (int i = 0; i < entities.size(); ++i) {
        if (entities[i]->id == id) {
            Entity* e = entities[i]; // keep for cleanup purposes ***TODO***
            entities.erase(entities.begin() + i);
            break;
        }
    }
}