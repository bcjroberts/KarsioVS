

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>

EntityManager::EntityManager() {

}

// Can globally access the EntityManager from anywhere
EntityManager * EntityManager::getInstance()
{
	if (!globalInstance) {
		globalInstance = new EntityManager;
	}
	return globalInstance;
}

EntityManager::~EntityManager() {

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

void EntityManager::registerEntity(Entity * e) {
	entities.push_back(e);
}

void EntityManager::fireGlobalEvent(Event * e) {
	for (int i = 0; i < entities.size(); ++i) {
		entities[i]->processEvent(e);
	}
}

void EntityManager::processFrameUpdate() {
	for (int i = 0; i < entities.size(); ++i) {
		entities[i]->processFrameUpdate();
	}
}

