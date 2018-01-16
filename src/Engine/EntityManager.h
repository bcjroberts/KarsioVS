#pragma once
#include "Entity.h"
#include "vector"
#include "Event.h"

class EntityManager
{
private:
	std::vector<Entity*> entities;
	static EntityManager* globalInstance;
public:
    EntityManager();
    void destroyEntity(int id);
	void registerEntity(Entity* e);
	void fireGlobalEvent(Event* e);
	void processFrameUpdate();
	static EntityManager* getInstance();
    ~EntityManager();
};

