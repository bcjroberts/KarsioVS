#include "WorldGenerator.h"
#include "../../engine/EntityManager.h"

WorldGenerator::WorldGenerator() {
}


WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::generateWorld() {

	// For now this is just the stuff copied from Core.
	// Creating crytsal entities. A value for the size of the crystal can be speicifed if wanted
	//Entity* crystalEntity1 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 55), 0.5f);
	//Entity* crystalEntity2 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 15), 1.0f);
	//Entity* crystalEntity3 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 25), 1.5f);
	//Entity* crystalEntity4 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 35), 2.0f);
	//Entity* crystalEntity5 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 45), 2.5f);

	// These crystals mark the ai path
	//Entity* crystalEntity6 = EntityManager::getInstance()->createCrystal(glm::vec3(45, 1.0f, 45), 0.5f);
	//Entity* crystalEntity7 = EntityManager::getInstance()->createCrystal(glm::vec3(-55, 1.0f, 55), 0.65f);
	//Entity* crystalEntity8 = EntityManager::getInstance()->createCrystal(glm::vec3(-55, 1.0f, -55), 0.85f);
	//Entity* crystalEntity9 = EntityManager::getInstance()->createCrystal(glm::vec3(55, 1.0f, -55), 1.0f);

	// Create some temporary walls around the outside to prevent people from escaping.
	Entity* wall1 = EntityManager::getInstance()->createBox(glm::vec3(-100, 2, 0), glm::vec3(2, 4, 100));
	Entity* wall2 = EntityManager::getInstance()->createBox(glm::vec3(100, 2, 0), glm::vec3(2, 4, 100));
	Entity* wall3 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, 100), glm::vec3(100, 4, 2));
	Entity* wall4 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, -100), glm::vec3(100, 4, 2));
	
	//Entity* wall5 = EntityManager::getInstance()->createBoulder(glm::vec3(15, 2, -15), glm::vec3(2, 4, 50));
	//Entity* wall6 = EntityManager::getInstance()->createBoulder(glm::vec3(65, 2, 25), glm::vec3(2, 4, 50));

	//Entity* wall7 = EntityManager::getInstance()->createBoulder(glm::vec3(55, 2, 25), glm::vec3(50, 4, 2));
	//Entity* wall8 = EntityManager::getInstance()->createBoulder(glm::vec3(-35, 2, 55), glm::vec3(2, 4, 50));
	std::vector<glm::vec3> positions;
	positions.clear();



	obstacles.push_back(wall1);
	obstacles.push_back(wall2);
	obstacles.push_back(wall3);
	obstacles.push_back(wall4);
	//obstacles.push_back(wall5);
	//obstacles.push_back(wall6);
	//obstacles.push_back(wall7);
	//obstacles.push_back(wall8);
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());     // provide seed
	/*
	for (int i = 0; i < 50; i++) {
		if (i % 5 == 0) {
			Entity* wall = EntityManager::getInstance()->createBoulder(glm::vec3(15, 2, -15 + i), glm::vec3(2.5, 2.5, 2.5));
			obstacles.push_back(wall);
		}
	}
	*/
	// create boulders
	int m = 0;
	for (int i = 0; i < 20; i++) {
		std::uniform_int_distribution<int> uid{ -9, 9 };
		// make sure they end in 5....
		int x = uid(dre) * 10 + 5;
		int z = uid(dre) * 10 + 5;
		while (std::find(positions.begin(), positions.end(), glm::vec3(x, 2, z)) != positions.end()) {
			std::uniform_int_distribution<int> uid{ -9, 9 };
			std::cout << "oh no, " << x << ", " << z << std::endl;
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		}
		std::uniform_int_distribution<int> uidb{ 0, 1 };
		m = uidb(dre) ? 1 : 0;	// 1 = expand in x, 0 = expand in z
		// make 5 boulders side by side in x or y
		for (int j = 0; j < 3; j++) {
			if (std::find(positions.begin(), positions.end(), glm::vec3(x, 2, z)) != positions.end() ||
				(x > 95 || x < -95) || (z > 95 || z < -95)) {
				// skip making obstacle
			}
			else {
				// choose to grow in x or y
				Entity* boulder = EntityManager::getInstance()->createBoulder(glm::vec3(x, 2, z), glm::vec3(4, 4, 4));
				obstacles.push_back(boulder);
				positions.push_back(glm::vec3(x, 2, z));
			}
			if (m == 1) {
				x += 10;
			}
			else {
				z += 10;
			}
		}
	}

	// create crystals of variable sizes
	for (int i = 0; i < 45; i++) {
		std::uniform_real_distribution<float> urd{ 0, 2.5};
		float resourceAmount = urd(dre);

		std::uniform_int_distribution<int> uid{ -9, 9 };
		// make sure they end in 5....
		int x = uid(dre) * 10 + 5;
		int z = uid(dre) * 10 + 5;
		while (std::find(positions.begin(), positions.end(), glm::vec3(x, 2, z)) != positions.end()) {
			std::uniform_int_distribution<int> uid{ -9, 9 };
			std::cout << "oh no, " << x << ", " << z << std::endl;
			x = uid(dre) * 10 + 5;
			z = uid(dre) * 10 + 5;
		}

		positions.push_back(glm::vec3(x, 2, z));

		Entity* crystal = EntityManager::getInstance()->createCrystal(glm::vec3(x, 2, z), resourceAmount);
		crystals.push_back(crystal);
	}
	/*
	crystals.push_back(crystalEntity1);
	crystals.push_back(crystalEntity2);
	//crystals.push_back(crystalEntity3);
	crystals.push_back(crystalEntity4);
	crystals.push_back(crystalEntity5);
	crystals.push_back(crystalEntity6);
	crystals.push_back(crystalEntity7);
	crystals.push_back(crystalEntity8);
	crystals.push_back(crystalEntity9);
	*/
	//for (int i = 1; i < 100; i++) {
	//	if (i % 5 == 0) {
	//		Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-70 + i, 1.f, 25.f), glm::vec3(.5f));
	//		obstacles.push_back(obstacle);
	//	}
	//}
	

	/// these mark edges of grid for clarity's sake so I can keep my sanity
	/*
	for (int i = 0; i < 200; i++) {
		if (i % 10 == 0 && i != 100) {
			Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-100 + i, 1.f, 0), glm::vec3(.5f));
			obstacles.push_back(obstacle);
		}
	}
	for (int i = 0; i < 200; i++) {
		if (i % 10 == 0 && i != 100) {
			Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(0, 1.f, -100 + i), glm::vec3(.5f));
			obstacles.push_back(obstacle);
		}
	}
	*/
	/// end sanity markers

	Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-100, 1.f, -100), glm::vec3(.5f));
	obstacles.push_back(obstacle);

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
