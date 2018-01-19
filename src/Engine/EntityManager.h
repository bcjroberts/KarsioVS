#pragma once

#include "vector"
#include "Event.h"
#include <glm/gtc/quaternion.hpp>
#include "../game/components/RendererComponent.h"

class Entity;

class EntityManager
{
private:
	std::vector<Entity*> entities;
	static EntityManager* globalInstance;
public:
    EntityManager();
	void destroyEntity(int id);
	Entity* createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	static EntityManager* getInstance();
    ~EntityManager();
};

