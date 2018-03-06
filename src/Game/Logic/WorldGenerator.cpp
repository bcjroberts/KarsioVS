#include "WorldGenerator.h"
#include "../../engine/EntityManager.h"

WorldGenerator::WorldGenerator() {
}


WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::generateWorld() {
	createWalls();

	std::vector<glm::vec3> positions;
	
	for (int i = 0; i < EntityManager::getInstance()->getEntities().size(); i++) {
		glm::vec3 pos = EntityManager::getInstance()->getEntities().at(i)->getCoarsePosition();
		positions.push_back(pos);
		positions.push_back(glm::vec3(pos.x + 1, pos.y, pos.z + 1));
		positions.push_back(glm::vec3(pos.x - 1, pos.y, pos.z + 1));
		positions.push_back(glm::vec3(pos.x + 1, pos.y, pos.z - 1));
		positions.push_back(glm::vec3(pos.x - 1, pos.y, pos.z - 1));
		positions.push_back(glm::vec3(pos.x, pos.y, pos.z + 1));
		positions.push_back(glm::vec3(pos.x + 1, pos.y, pos.z));
		positions.push_back(glm::vec3(pos.x, pos.y, pos.z - 1));
		positions.push_back(glm::vec3(pos.x - 1, pos.y, pos.z));
	}

	createObstacles(&positions);
	createCrystals(&positions);

	/// these mark edges of grid for clarity's sake so I can keep my sanity
	//for (int i = 0; i < 200; i++) {
	//	if (i % 10 == 0 && i != 100) {
	//		Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-100 + i, 1.f, 0), glm::vec3(.5f));
	//		obstacles.push_back(obstacle);
	//	}
	//}
	//for (int i = 0; i < 200; i++) {
	//	if (i % 10 == 0 && i != 100) {
	//		Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(0, 1.f, -100 + i), glm::vec3(.5f));
	//		obstacles.push_back(obstacle);
	//	}
	//}
	/// end sanity markers
}

void WorldGenerator::createObstacles(std::vector<glm::vec3> *positions) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());

	// create boulders
	int m = 0;
	for (int i = 0; i < 20; i++) {
		std::uniform_int_distribution<int> uid{ -9, 9 };
		// make sure they end in 5....
		int x = uid(dre) * 10 + 5;
		int z = uid(dre) * 10 + 5;
		while (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) != positions->end()) {
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		}
		std::uniform_int_distribution<int> uidb{ 0, 1 };

		// make 3 boulders side by side
		for (int j = 0; j < 3; j++) {
			m = uidb(dre) ? 1 : 0;	// 1 = expand in x, 0 = expand in z
			if (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) != positions->end() ||
				(x > 95 || x < -95) || (z > 95 || z < -95)) {
				// skip making obstacle
			}
			else {
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

void WorldGenerator::createCrystals(std::vector<glm::vec3> *positions) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());

	// create crystals of variable sizes
	for (int i = 0; i < 45; i++) {
		std::uniform_real_distribution<float> urd{ 0.5, 1.5 };
		float resourceAmount = urd(dre);

		std::uniform_int_distribution<int> uid{ -8, 8 };
		// make sure they end in 5....
		int x = uid(dre) * 10 + 5;
		int z = uid(dre) * 10 + 5;
		while (std::find(positions->begin(), positions->end(), glm::vec3(x, 2, z)) != positions->end()) {
			//std::cout << "oh no, " << x << ", " << z << std::endl;
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		}

		positions->push_back(glm::vec3(x, 2, z));

		Entity* crystal = EntityManager::getInstance()->createCrystal(glm::vec3(x, 2, z), resourceAmount);
		crystals.push_back(crystal);
	}
}

void WorldGenerator::createWalls() {
	// create edge walls
	float boulderSize = 2.5f;
	int bounds = 100;
	for (int i = 0; i < 200; i++) {
		if (i % 10 == 0) {
			Entity* boulder1 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds + i, 2, -bounds), glm::vec3(boulderSize));
			obstacles.push_back(boulder1);

			Entity* boulder2 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds + i, 2, bounds), glm::vec3(boulderSize));
			obstacles.push_back(boulder2);
		}
	}

	for (int i = 0; i < 200; i++) {
		if (i % 10 == 0) {
			Entity* boulder1 = EntityManager::getInstance()->createWallBoulder(glm::vec3(bounds, 2, -bounds + i), glm::vec3(boulderSize));
			obstacles.push_back(boulder1);

			Entity* boulder2 = EntityManager::getInstance()->createWallBoulder(glm::vec3(-bounds, 2, -bounds + i), glm::vec3(boulderSize));
			obstacles.push_back(boulder2);
		}
	}
}

std::vector<Entity*>* WorldGenerator::getObstacles() {
	return &obstacles;
}

std::vector<Entity*>* WorldGenerator::getCrystals() {
	return &crystals;
}

int WorldGenerator::getCrystalSize() {
	return crystals.size();
}
