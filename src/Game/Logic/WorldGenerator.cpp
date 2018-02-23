#include "WorldGenerator.h"
#include "../../engine/EntityManager.h"


WorldGenerator::WorldGenerator() {
}


WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::generateWorld() {

	// For now this is just the stuff copied from Core.
	// Creating crytsal entities. A value for the size of the crystal can be speicifed if wanted
	Entity* crystalEntity1 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 10), 0.5f);
	Entity* crystalEntity2 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 15), 1.0f);
	Entity* crystalEntity3 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 25), 1.5f);
	Entity* crystalEntity4 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 35), 2.0f);
	Entity* crystalEntity5 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 45), 2.5f);

	// These crystals mark the ai path
	Entity* crystalEntity6 = EntityManager::getInstance()->createCrystal(glm::vec3(40, 1.0f, 40), 0.5f);
	Entity* crystalEntity7 = EntityManager::getInstance()->createCrystal(glm::vec3(-50, 1.0f, 50), 0.65f);
	Entity* crystalEntity8 = EntityManager::getInstance()->createCrystal(glm::vec3(-50, 1.0f, -50), 0.85f);
	Entity* crystalEntity9 = EntityManager::getInstance()->createCrystal(glm::vec3(50, 1.0f, -50), 1.0f);

	// Create some temporary walls around the outside to prevent people from escaping.
	Entity* wall1 = EntityManager::getInstance()->createBox(glm::vec3(-100, 2, 0), glm::vec3(2, 4, 100));
	Entity* wall2 = EntityManager::getInstance()->createBox(glm::vec3(100, 2, 0), glm::vec3(2, 4, 100));
	Entity* wall3 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, 100), glm::vec3(100, 4, 2));
	Entity* wall4 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, -100), glm::vec3(100, 4, 2));

	obstacles.push_back(crystalEntity1);
	obstacles.push_back(crystalEntity2);
	obstacles.push_back(crystalEntity3);
	obstacles.push_back(crystalEntity4);
	obstacles.push_back(crystalEntity5);
	//obstacles.push_back(crystalEntity6);
	//obstacles.push_back(crystalEntity7);
	obstacles.push_back(crystalEntity8);
	obstacles.push_back(crystalEntity9);

	crystals.push_back(crystalEntity1);
	crystals.push_back(crystalEntity2);
	crystals.push_back(crystalEntity3);
	crystals.push_back(crystalEntity4);
	crystals.push_back(crystalEntity5);
	crystals.push_back(crystalEntity6);
	crystals.push_back(crystalEntity7);
	crystals.push_back(crystalEntity8);
	crystals.push_back(crystalEntity9);

	for (int i = 1; i < 100; i++) {
		if (i % 5 == 0) {
			Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-70 + i, 1.f, 25.f), glm::vec3(.5f));
			obstacles.push_back(obstacle);
		}
	}
}

std::vector<Entity*>* WorldGenerator::getObstacles() {
	return &obstacles;
}

std::vector<Entity*>* WorldGenerator::getCrystals() {
	return &crystals;
}