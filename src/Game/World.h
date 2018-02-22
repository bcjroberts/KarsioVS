#pragma once

#ifndef PROJECTKARSIO_WORLD_H
#define PROJECTKARSIO_WORLD_H

#include "../Engine/EntityManager.h"
#include "../Engine/Entity.h"
#include "Logic\AStar.h"
#include <vector>

class World {
public:
	AStar::Generator gen;
	std::vector<Entity*> walls;
	std::vector<Entity*> destructables;
	std::vector<Entity*> crystals;

	World();
	void generateWorld();
	AStar::Generator getPositions();
	void crystalCollider(Entity* crystal);
	~World();
};

#endif //PROJECTKARSIO_	WORLD_H