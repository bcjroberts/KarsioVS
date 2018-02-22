
#include "World.h"

World::World() {
}

void World::generateWorld() {
	
	// Create some obstacles										// location				size
	Entity* obstacle1 = EntityManager::getInstance()->createBox(glm::vec3(5.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle2 = EntityManager::getInstance()->createBox(glm::vec3(6.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle3 = EntityManager::getInstance()->createBox(glm::vec3(7.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle4 = EntityManager::getInstance()->createBox(glm::vec3(8.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle5 = EntityManager::getInstance()->createBox(glm::vec3(9.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle6 = EntityManager::getInstance()->createBox(glm::vec3(10.f, 1.f, 25.f), glm::vec3(1.f));

	// Creating crytsal entities. A value for the size of the crystal can be speicifed if wanted
	Entity* crystalEntity1 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 5), 0.5f);
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

	walls.emplace_back(obstacle1);
	walls.emplace_back(obstacle2);
	walls.emplace_back(obstacle3);
	walls.emplace_back(obstacle4);
	walls.emplace_back(obstacle5);
	walls.emplace_back(obstacle6);
	
	crystals.emplace_back(crystalEntity1);
	crystals.emplace_back(crystalEntity2);
	crystals.emplace_back(crystalEntity3);
	crystals.emplace_back(crystalEntity4);
	crystals.emplace_back(crystalEntity5);
	crystals.emplace_back(crystalEntity6);
	crystals.emplace_back(crystalEntity7);
	crystals.emplace_back(crystalEntity8);
	crystals.emplace_back(crystalEntity9);
	
	for (int i = 1; i < 100; i++) {
		Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-70 + i, 1.f, 25.f), glm::vec3(.5f));
		walls.emplace_back(obstacle);
	}

	gen.setWorldSize({ 15, 15 });
	
	float x, y;
	for (int i = 0; i < walls.size() - 1; i++) {
		x = walls[i]->getCoarsePosition().x;
		y = walls[i]->getCoarsePosition().z;
		gen.addCollision(vec2(x, y));
	}
	for (int i = 0; i < crystals.size() - 1; i++) {
		x = crystals[i]->getCoarsePosition().x;
		y = crystals[i]->getCoarsePosition().z;
		gen.addCrystal(vec2(x, y));
	}
	
}

AStar::Generator World::getPositions() {
	return gen;
}



World::~World() = default;
