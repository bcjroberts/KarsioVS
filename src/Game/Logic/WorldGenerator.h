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
	void createWalls();
	void createObstacles(std::vector<glm::vec3> *positions);
	void createCrystals(std::vector<glm::vec3> *positions);
	std::vector<Entity*>* getObstacles();
	std::vector<Entity*>* getCrystals();
	int getCrystalSize();
};

