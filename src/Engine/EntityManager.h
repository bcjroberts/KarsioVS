#pragma once

#include "vector"
#include "Event.h"
#include <glm/gtc/quaternion.hpp>

class Entity;

class EntityManager
{
private:
	std::vector<Entity*> entities;
	static EntityManager* globalInstance;
public:
    EntityManager();
	void destroyEntity(int id);
	void fireGlobalEvent(Event* e);
	Entity* createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	void processFrameUpdate();
	static EntityManager* getInstance();
    ~EntityManager();
};

