

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>
#include "Entity.h"

// Initialize the Entity Manager global pointer.
EntityManager *EntityManager::globalInstance = nullptr;

EntityManager::EntityManager() = default;

// Can globally access the EntityManager from anywhere
EntityManager * EntityManager::getInstance()
{
	if (!globalInstance) {
		globalInstance = new EntityManager;
	}
	return globalInstance;
}

EntityManager::~EntityManager() = default;

void EntityManager::destroyEntity(int id) {
	for (uint32_t i = 0; i < entities.size(); ++i) {
		if (entities[i]->id == id) {
			Entity* e = entities[i]; // keep for cleanup purposes
			entities.erase(entities.begin() + i);
			delete e;
			break;
		}
	}
}

Entity* EntityManager::createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
	Entity* e = new Entity(position, rotation, scale);
	entities.push_back(e);
	return e;
}
