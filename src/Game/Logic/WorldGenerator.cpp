#include "WorldGenerator.h"
#include "../../engine/EntityManager.h"
#include "../../Engine/Core.h"
#include "../Components/UpgradeComponent.h"
#include "../Components/AIComponent.h"

using namespace glm;
WorldGenerator::WorldGenerator() {
}

WorldGenerator* WorldGenerator::globalInstance = nullptr;

WorldGenerator::~WorldGenerator() = default;

WorldGenerator* WorldGenerator::getInstance() {
	if (!globalInstance) {
		globalInstance = new WorldGenerator;
	}
	return globalInstance;
}

void WorldGenerator::generateWorld() {
	int gridSize = 30;
	std::vector<glm::vec3> positions;
	
    obstacles.clear();
    crystals.clear();
    emptyCrystals.clear();
    if (aStar != nullptr)
        delete aStar;
    aStar = new AStar::Generator;

	// for the sake of the id's being nice numbers, vehicles will be generated before anything else
	createVehicles(&positions, gridSize);
	Entity* groundPlane = EntityManager::getInstance()->createGroundPlane();
	createWalls(gridSize);
	createObstacles(&positions, gridSize);
	createCrystals(&positions, gridSize);
	generateGrid(gridSize);
}

void WorldGenerator::createVehicles(std::vector<glm::vec3> *positions, int gridSize) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> uid{ -(gridSize - 5), gridSize - 5 };
	
	int x, z;
	for (unsigned int i = 0; i < 10; i++) {
		do {
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		} while (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) != positions->end());

		if (i == 0) {
			Entity* playerVehicle = EntityManager::getInstance()->createPlayerVehicleEntity(glm::vec3(x, 10, z));
		}
		else {
			Entity* aiVehicle = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(x, 10, z));
            
            
			// make this the big scary strong one
			if (i == 1) {
				static_cast<UpgradeComponent*>(aiVehicle->getComponent(UPGRADE))->setPreUpgradeLevels(3, 5, 5, 5);
				static_cast<AIComponent*>(aiVehicle->getComponent(AI))->setPersonality(AIComponent::KILL_ONLY);
			}

			// make some midranged, midscary ones
			else if (i < 6) {
				static_cast<UpgradeComponent*>(aiVehicle->getComponent(UPGRADE))->setPreUpgradeLevels(2, 3, 3, 3);
				static_cast<AIComponent*>(aiVehicle->getComponent(AI))->setPersonality(AIComponent::RANDOM);
			}
		}

		// actual position
		positions->push_back(glm::vec3(x, 2, z));	

		// diagonal
		positions->push_back(glm::vec3(x + 10, 2, z + 10));
		positions->push_back(glm::vec3(x - 10, 2, z + 10));
		positions->push_back(glm::vec3(x + 10, 2, z - 10));
		positions->push_back(glm::vec3(x - 10, 2, z - 10));

		// adjacent
		positions->push_back(glm::vec3(x, 2, z + 10));
		positions->push_back(glm::vec3(x, 2, z - 10));
		positions->push_back(glm::vec3(x + 10, 2, z));
		positions->push_back(glm::vec3(x - 10, 2, z));
	}
}

void WorldGenerator::generateGrid(int gridSize) {
	aStar->setWorldSize({ gridSize, gridSize });
	
	float x, y;
	for (unsigned int i = 0; i < obstacles.size() - 1; i++) {
		x = obstacles[i]->getCoarsePosition().x;
		y = obstacles[i]->getCoarsePosition().z;
		aStar->addCollision(vec2(x, y));
	}
	for (unsigned int i = 0; i < crystals.size() - 1; i++) {
		x = crystals[i]->getCoarsePosition().x;
		y = crystals[i]->getCoarsePosition().z;
		aStar->addCrystal(vec2(x, y));
	}
}


void WorldGenerator::createObstacles(std::vector<glm::vec3> *positions, int gridSize) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());

	// create boulders
	int m = 0;
	for (int i = 0; i < 50; i++) {
		std::uniform_int_distribution<int> uid{ -(gridSize - 1), gridSize - 1 };
		// make sure they end in 5....
		int x, z;
		do {
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		} while (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) != positions->end());

		std::uniform_int_distribution<int> uidb{ 0, 1 };
		// make 3 boulders side by side
		for (int j = 0; j < 3; j++) {
			m = uidb(dre) ? 1 : 0;	// 1 = expand in x, 0 = expand in z
			if (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) == positions->end()) {
				// choose to grow in x or y
				Entity* boulder = EntityManager::getInstance()->createBoulder(glm::vec3(x, 2, z), glm::vec3(3.8f));
				obstacles.push_back(boulder);
				positions->push_back(glm::vec3(x, 2, z));
			}
			if (m == 1) {
				x += 10;
			}
			else {
				z += 10;
			}
		}
	}
}

void WorldGenerator::createCrystals(std::vector<glm::vec3> *positions, int gridSize) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());

	// create crystals of variable sizes
	for (int i = 0; i < 45; i++) {
		std::uniform_real_distribution<float> urd{ 0.5, 2.5 };
		float resourceAmount = urd(dre);

		std::uniform_int_distribution<int> uid{ -(gridSize - 2), gridSize - 2 };
		// make sure they end in 5....
		int x, z;
		do {
			x = uid(dre);
			z = uid(dre);
		} while (std::find(positions->begin(), positions->end(), glm::vec3(x * 10 + 5, 2, z * 10 + 5)) != positions->end());

		x = x * 10 + 5;
		z = z * 10 + 5;
		positions->push_back(glm::vec3(x, 2, z));

		Entity* crystal = EntityManager::getInstance()->createCrystal(glm::vec3(x, 2, z), resourceAmount);
		crystals.push_back(crystal);
	}
}

void WorldGenerator::createWalls(int gridSize) {
	// create edge walls
	float boulderSize = 2.5f;
	int bounds = gridSize * 10;
	for (int i = 0; i < gridSize * 20; i++) {
		if (i % 10 == 0) {
			Entity* boulder1 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds + i, 2, -bounds), glm::vec3(boulderSize));
			obstacles.push_back(boulder1);

			Entity* boulder2 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds + i, 2, bounds), glm::vec3(boulderSize));
			obstacles.push_back(boulder2);
		}
	}

	for (int i = 0; i < gridSize * 20; i++) {
		if (i % 10 == 0) {
			Entity* boulder1 = EntityManager::getInstance()->createWallBoulder(glm::vec3(bounds, 2, -bounds + i), glm::vec3(boulderSize));
			obstacles.push_back(boulder1);

			Entity* boulder2 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds, 2, -bounds + i), glm::vec3(boulderSize));
			obstacles.push_back(boulder2);
		}
	}
}

AStar::Generator* WorldGenerator::getGrid() {
	return aStar;
}

std::vector<Entity*>* WorldGenerator::getObstacles() {
	return &obstacles;
}

std::vector<Entity*>* WorldGenerator::getCrystals() {
	return &crystals;
}

void WorldGenerator::removeCrystal(Entity* entity) {
	auto it = std::find(crystals.begin(), crystals.end(), entity);
	if (it != crystals.end()) {
		crystals.erase(it);
	}
	aStar->removeCrystal(glm::vec2(entity->getCoarsePosition().x, entity->getCoarsePosition().z));

	// mark area for regeneration
	emptyCrystals.push_back(glm::vec2(entity->getPosition().x, entity->getPosition().z));
}

void WorldGenerator::createSingleCrystal(glm::vec2 position) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> urd{ 0.5, 2.5 };
	float resourceAmount = urd(dre);

	Entity* crystal = EntityManager::getInstance()->createCrystal(glm::vec3(position.x, 2, position.y), resourceAmount);
	crystals.push_back(crystal);
	aStar->addCrystal(vec2(crystal->getCoarsePosition().x, crystal->getCoarsePosition().z));
}

void WorldGenerator::regenerateCrystal() {
	if (!emptyCrystals.empty()) {
		int i = rand() % emptyCrystals.size();
		if (Core::simtimeSinceStartup - lastRegenTime > 5.f) {
			createSingleCrystal(emptyCrystals[i]);
			emptyCrystals.erase(emptyCrystals.begin() + i);
			lastRegenTime = Core::simtimeSinceStartup;
		}
	}
}