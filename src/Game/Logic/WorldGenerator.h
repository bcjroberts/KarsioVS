#pragma once
#include "../../Engine/Entity.h"
#include <random>
#include <chrono> 

class WorldGenerator {
private:
	std::vector<Entity*> obstacles;
	std::vector<Entity*> crystals;
public:
	WorldGenerator();
	~WorldGenerator();
	void generateWorld();
	std::vector<Entity*>* getObstacles();
	std::vector<Entity*>* getCrystals();
	int getCrystalSize();
};

