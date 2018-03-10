#pragma once
#include "../../Engine/Entity.h"
#include <random>
#include <chrono> 
#include "AStar.h"

class WorldGenerator {
private:
	static WorldGenerator* globalInstance;
	std::vector<Entity*> obstacles;
	std::vector<Entity*> crystals;
	AStar::Generator aStar;
public:
	AStar::Generator* getGrid();
	WorldGenerator* getInstance();
	WorldGenerator();
	~WorldGenerator();
	void generateGrid(int gridSize);
	void generateWorld();
	void createWalls(int gridSize);
	void createObstacles(std::vector<glm::vec3> *positions, int gridSize);
	void createCrystals(std::vector<glm::vec3> *positions, int gridSize);
	std::vector<Entity*>* getObstacles();
	std::vector<Entity*>* getCrystals();
	int getCrystalSize();
};

