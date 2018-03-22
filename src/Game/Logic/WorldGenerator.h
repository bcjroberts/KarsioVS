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
	std::vector<glm::vec2> emptyCrystals;
	AStar::Generator aStar;
	float lastRegenTime = 0.f;
public:
	AStar::Generator* getGrid();
	static WorldGenerator* getInstance();
	WorldGenerator();
	~WorldGenerator();
	void generateGrid(int gridSize);
	void generateWorld();
	void createVehicles(std::vector<glm::vec3> *positions, int gridSize);
	void createWalls(int gridSize);
	void createObstacles(std::vector<glm::vec3> *positions, int gridSize);
	void createCrystals(std::vector<glm::vec3> *positions, int gridSize);
	std::vector<Entity*>* getObstacles();
	std::vector<Entity*>* getCrystals();
	void removeCrystal(Entity* entity);
	void regenerateCrystal();
	void createSingleCrystal(glm::vec2 position);
};

