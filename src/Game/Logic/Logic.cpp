//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>
#include "../../Engine/Core.h"
#include <random>
#include <chrono>
#include "../../Engine/AudioPaths.h"

Logic::Logic(){
};
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}



int playerChassisLevelTextId = -1;
int playerArmorLevelTextId = -1;
int playerGunLevelTextId = -1;
int playerRamLevelTextId = -1;

int playerChassisUpgradeTextId = -1;
int playerArmorUpgradeTextId = -1;
int playerGunUpgradeTextId = -1;
int playerRamUpgradeTextId = -1;

void Logic::createPlayerUpgradeUI() {
    playerChassisLevelTextId = Core::renderEngine->ui->addText("Chassis Lvl: 1", 350, 30, 0.5, glm::vec3(1, 1, 0));
    playerArmorLevelTextId = Core::renderEngine->ui->addText("Armor Lvl: 1", 650, 30, 0.5, glm::vec3(1, 1, 0));
    playerGunLevelTextId = Core::renderEngine->ui->addText("Gun Lvl: 1", 950, 30, 0.5, glm::vec3(1, 1, 0));
    playerRamLevelTextId = Core::renderEngine->ui->addText("Ram Lvl: 1", 1250, 30, 0.5, glm::vec3(1, 1, 0));

    // This holds the text for when upgrades are available
    playerChassisUpgradeTextId = Core::renderEngine->ui->addText("", 350, 50, 0.5, glm::vec3(1, 1, 0));
    playerArmorUpgradeTextId = Core::renderEngine->ui->addText("", 650, 50, 0.5, glm::vec3(1, 1, 0));
    playerGunUpgradeTextId = Core::renderEngine->ui->addText("", 950, 50, 0.5, glm::vec3(1, 1, 0));
    playerRamUpgradeTextId = Core::renderEngine->ui->addText("", 1250, 50, 0.5, glm::vec3(1, 1, 0));
}
//void Logic::playerMovement(vehicleInput *newMovement, Entity* targetEnt) {
void Logic::playerMovement(Entity* targetEnt) {
	ControllableComponent* con = static_cast<ControllableComponent*>(targetEnt->getComponent(CONTROLLABLE));
	DriveComponent* temp = static_cast<DriveComponent*>(targetEnt->getComponent(DRIVE));
    HealthComponent* hc = static_cast<HealthComponent*>(targetEnt->getComponent(HEALTH));
	//if (temp) temp->setInputs(*newMovement);
    if (!hc->isDead()) {
	    con->getInput();
    	temp->setInputs(con->input);
    } else {
        temp->setInputs(0, 0, 0, 0, false, false);
        return;
    }

    if (temp->getFlip() && canVehicleFlip(targetEnt)) {
        flipVehicle(targetEnt);
    }

    if (temp->getShooting()) {
        static_cast<WeaponComponent*>(targetEnt->getComponent(WEAPON))->fireWeapon();
    }

    UpgradeComponent* uc = static_cast<UpgradeComponent*>(targetEnt->getComponent(UPGRADE));
    std::ostringstream oss;
    oss << "Chassis lvl: " << uc->getChassisLevel();
    std::string chassisLevel = oss.str();
    Core::renderEngine->ui->modifyText(playerChassisLevelTextId, &chassisLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Armor lvl: " << uc->getArmorLevel();
    std::string armorLevel = oss.str();
    Core::renderEngine->ui->modifyText(playerArmorLevelTextId, &armorLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Gun lvl: " << uc->getGunLevel();
    std::string gunLevel = oss.str();
    Core::renderEngine->ui->modifyText(playerGunLevelTextId, &gunLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Ram lvl: " << uc->getRamLevel();
    std::string ramLevel = oss.str();
    Core::renderEngine->ui->modifyText(playerRamLevelTextId, &ramLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    // Now check for upgrades
    glm::vec3 upgradeColor(0,1,0);
    glm::vec3 defaultColor(1,1,0);
    std::string empty = "";
    if (uc->canUpgradeType(CHASSIS_UPGRADE)) {
        std::string chassisUpgrade = "+ Press 3/DPAD Up +";
        Core::renderEngine->ui->modifyText(playerChassisUpgradeTextId, &chassisUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeChassis) {
            uc->upgradeVehicle(CHASSIS_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(playerChassisUpgradeTextId, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(ARMOR_UPGRADE)) {
        std::string armorUpgrade = "+ Press 4/DPAD Right +";
        Core::renderEngine->ui->modifyText(playerArmorUpgradeTextId, &armorUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeArmor) {
            uc->upgradeVehicle(ARMOR_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(playerArmorUpgradeTextId, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(GUN_UPGRADE)) {
        std::string gunUpgrade = "+ Press 5/DPAD Down +";
        Core::renderEngine->ui->modifyText(playerGunUpgradeTextId, &gunUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeGun) {
            uc->upgradeVehicle(GUN_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(playerGunUpgradeTextId, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(RAM_UPGRADE)) {
        std::string ramUpgrade = "+ Press 6/DPAD Left +";
        Core::renderEngine->ui->modifyText(playerRamUpgradeTextId, &ramUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeRam) {
            uc->upgradeVehicle(RAM_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(playerRamUpgradeTextId, &empty, nullptr, nullptr, nullptr, nullptr);
    }
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

    // Check if the distance is less than some threshold. If so, then we have "arrived at the waypoint", head to the next waypoint.
    if (glm::distance(ai->getCurrentWaypoint(), entity->getCoarsePosition()) < 1.0f) {
        ai->nextWaypoint();
    }	
	
    // Now we need to determine where to steer based on where the point is. Currently does not support reverse.
    DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
    float steering = 0.0f;
    float accel = 0.0f;
    float oangle = glm::orientedAngle(glm::normalize(ai->getCurrentWaypoint() - entity->getCoarsePosition()),entity->getForwardVector(), glm::vec3(0,1,0));
	
	// if at last waypoint, move directly towards goal
	if (ai->currentWaypointIndex == ai->path.size() - 1) {
		oangle = glm::orientedAngle(glm::normalize(ai->getCurrentWaypoint() - entity->getPosition()), entity->getForwardVector(), glm::vec3(0, 1, 0));
	}

	// get speed of vehicle to make it slow down while turning
	glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(entity->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());

	float speed = glm::dot(velocity, entity->getForwardVector());

	// TODO this should be refined so that the AI doesn't spin out
    if (oangle > .1f) {
        steering = -1.0f;
		
    } else if (oangle < -.1f){
        steering = 1.0f;
    }
	// slow down when turning
	if (speed > 15 && (steering != 0.0f)) {
		accel = 0.0f;
	} else {
        accel = 1.0f;
	}
	// accel, reverse, handbrake, steering
    aiDrive->setInputs(accel, 0.0f, 0.0f, steering);
}

bool Logic::canVehicleFlip(Entity* vehicle) const {

    // In order to determine if a vehicle is allowed to flip, perform 2 checks:
    // First, that the up vector for the vehicle is not within a reasonable range of the normal up vector
    // Second, that the time since the last flip is larger than some threshold.

    float oangle = glm::dot(vehicle->getUpVector(), glm::vec3(0, 1, 0));

    // If less than some threshold, then condition 1 is met.
    if (oangle < 0.9f) {
        // Now we check condition 2. If we havent flipped in 2.5 seconds, allow flipping.
        if (Core::simtimeSinceStartup - static_cast<DriveComponent*>(vehicle->getComponent(DRIVE))->previousFlipTime > 2.5f) {
            return true;
        }
    }
    
    return false;
}

float flipForceValues[] = {6000.f, 10000.f, 18000.f};

void Logic::flipVehicle(Entity* vehicle) const{
    glm::vec3 torqueDirection = vehicle->getForwardVector();
    float oangle = glm::dot(vehicle->getRightVector(), glm::vec3(0, 1, 0)) * -1.f;
    // Is the vehicle upside down?
    if (abs(oangle) < 0.1f) {
        torqueDirection = torqueDirection * 1.5f;
    }
    else { // Otherwise flip in the best direction
        torqueDirection = torqueDirection * oangle;
    }
	
    int chassisLevel = static_cast<UpgradeComponent*>(vehicle->getComponent(UPGRADE))->getChassisLevel();
    const physx::PxVec3 verticalForce = physx::PxVec3(0, 1, 0) * flipForceValues[chassisLevel - 1] * 2.f;
    const physx::PxVec3 torqueForce = PhysicsEngine::toPxVec3(torqueDirection) * flipForceValues[chassisLevel - 1];

    static_cast<PhysicsComponent*>(vehicle->getComponent(PHYSICS))->getRigidBody()->addForce(verticalForce, physx::PxForceMode::eIMPULSE, true);
    static_cast<PhysicsComponent*>(vehicle->getComponent(PHYSICS))->getRigidBody()->addTorque(torqueForce, physx::PxForceMode::eIMPULSE, true);

    // Set the time since the last flip attempt was made.
    static_cast<DriveComponent*>(vehicle->getComponent(DRIVE))->previousFlipTime = Core::simtimeSinceStartup;
}

void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}

glm::vec3 getCoarsePosition(glm::vec3 position) {
	float x = position.x;
	float y = position.y;
	float z = position.z;
	float gridSize = 20; // larger size = larger grid

	x = floor(x / gridSize);
	y = floor(y / gridSize);
	z = floor(z / gridSize);
	return glm::vec3(x, y, z);
}

void Logic::findPath(AStar::Generator* generator, Entity* entity, glm::vec3 goal) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	
	// temporary
	if (goal.x < -301 && goal.z < -301) {
		ai->path.resize(0);
		ai->state = DECIDING;
		return;
	}
	glm::vec3 coarseStart = getCoarsePosition(entity->getPosition());
	glm::vec3 coarseGoal = getCoarsePosition(goal);	

	if (glm::distance(ai->goal->getPosition(), entity->getPosition()) < 15.0f) {
		ai->path.resize(1);
		ai->path[0] = glm::vec3(goal.x, 0, goal.z); // add exact position
		ai->clearWaypoints();
		ai->setWaypoints(ai->path);
		return;
	}

	ai->path = generator->findPath({ glm::vec2(coarseStart.x, coarseStart.z) }, { glm::vec2(coarseGoal.x, coarseGoal.z) });

	if (ai->path.empty()) {
		//std::cout << entity->id << " path not found" << std::endl;
		ai->state = DECIDING;
		return;
	}

	ai->path.pop_back();	// remove current position
	if (ai->path.size() < 1) {
		ai->path.resize(1);
	}
	ai->path[0] = glm::vec3(goal.x, 0, goal.z); // add exact position
	ai->clearWaypoints();
	ai->setWaypoints(ai->path);
	
	//std::cout << "path for " << entity->id << std::endl;
	//for (int i = ai->path.size() - 1; i > -1; i--) {
	//	std::cout << ai->path[i].x << " " << ai->path[i].z << std::endl;
	//}	
}

void Logic::mine(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
	
	// prevent orbiting around the crystal
	ai->orbiting++;
	if (ai->orbiting > 200) {
		// give up and do something else you stupid AI
		ai->state = DECIDING;
		ai->orbiting = 0;
	}
	if (ai->getMinedID() == ai->goal->id) {
		// if it hit crystal and didn't break it, probably won't be able to break it anyways
		ai->state = DECIDING;
		ai->setMinedID(-2);
	}
	else {
		float oangle = glm::orientedAngle(glm::normalize(ai->getCurrentWaypoint() - entity->getCoarsePosition()), entity->getForwardVector(), glm::vec3(0, 1, 0));
		float steering = 0;
		float accel = 1;
		if (oangle > .1f) {
			steering = -1.0f;

		}
		else if (oangle < -.1f) {
			steering = 1.0f;
		}
		// no slowing doooown
		// accel, reverse, handbrake, steering
		aiDrive->setInputs(accel, 0.0f, 0.0f, steering);
		//aiMovement(entity);
	}
}

void Logic::attack(Entity* goal, Entity* entity) {
	std::vector<Entity*> e = EntityManager::getInstance()->getEntities();

	// check if entity is still alive
	//if ((std::find(e.begin(), e.end(), static_cast<AIComponent*>(entity->getComponent(AI))->goal) == e.end())) {
	//	static_cast<AIComponent*>(entity->getComponent(AI))->state = DECIDING;
	//	return;
	//}

    // Determine if we should shoot by seeing if we are almost facing the direction of the player.
    float oangle = glm::orientedAngle(glm::normalize(goal->getPosition() - entity->getPosition()),entity->getForwardVector(), glm::vec3(0,1,0));
	// stop driving and attack
    if (abs(oangle) < 0.5f) {
        static_cast<WeaponComponent*>(entity->getComponent(WEAPON))->fireWeapon();
    }
	aiMovement(entity);
}

bool Logic::checkStuck(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(entity->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());
	float magnitude = glm::length(velocity);

    // First thing we check is if the Vehicle can flip
    if(canVehicleFlip(entity)) {
        flipVehicle(entity);
    }

	if (abs(magnitude) < 0.5f) {
		ai->notMoving++;
	} else {
		ai->notMoving = 0;
	}
	// speed has to be 0 for a while
	if (ai->notMoving > 100 && ai->state != STUCK) {
		if (ai->prevstate == -1) {
			ai->prevstate = ai->state;
		}
		ai->state = STUCK;
		ai->prevPos = entity->getPosition();
		return true;
	} else {
		return false;
	}
}

void Logic::unstuck(Entity* entity, AStar::Generator* generator) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
	
	if (glm::distance(ai->prevPos, entity->getPosition()) < 20.0f) {
		aiDrive->setInputs(0.0f, 1.0f, 0.0f, 0.0f);
	}
	else {
		findPath(generator, entity, ai->goal->getPosition());
		if (ai->path.size() > 0) {
			ai->state = ai->prevstate;
			ai->notMoving = 0;
		}
	}
}

int Logic::randomNum(int min, int max) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> uid{ min, max };
	return uid(dre);
}

void Logic::upgrade(Entity* entity) {
	// upgrade based on 'personality'
	// KILL_ONLY does not upgrade
	UpgradeComponent* uc = static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE));
	UpgradeType type;

	switch (static_cast<AIComponent*>(entity->getComponent(AI))->getPersonality()) {
	case AIComponent::RANDOM:
		type = UpgradeType(randomNum(0, 3));
		while (!uc->canUpgradeType(type)) {
			type = UpgradeType(randomNum(0, 3));
		}
		uc->upgradeVehicle(type);
		break;
	case AIComponent::GRINDER:
		type = UpgradeType(randomNum(0, 3));
		while (!uc->canUpgradeType(type)) {
			type = UpgradeType(randomNum(0, 3));
		}
		uc->upgradeVehicle(type);
		break;
	case AIComponent::GLASS_CANNON:
		if (uc->canUpgradeType(UpgradeType::GUN_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::GUN_UPGRADE);
		}
		else if (uc->canUpgradeType(UpgradeType::RAM_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::RAM_UPGRADE);
		}
		else if (uc->canUpgradeType(UpgradeType::CHASSIS_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::CHASSIS_UPGRADE);
		}
		break;
	case AIComponent::DEFENSIVE:
		if (uc->canUpgradeType(UpgradeType::ARMOR_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::ARMOR_UPGRADE);
		}
		else if (uc->canUpgradeType(UpgradeType::CHASSIS_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::CHASSIS_UPGRADE);
		}
		else if (uc->canUpgradeType(UpgradeType::RAM_UPGRADE)) {
			uc->upgradeVehicle(UpgradeType::RAM_UPGRADE);
		}
		else {
			uc->upgradeVehicle(UpgradeType::GUN_UPGRADE);
		}
		break;
	}
}

void Logic::decide(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	switch (ai->getPersonality()) {
	case AIComponent::RANDOM:
		if (!static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->fullyUpgraded()) {
			ai->state = randomNum(0, 1) ? FINDING_CRYSTAL : FINDING_PLAYER;
		}
		else {
			ai->state = FINDING_PLAYER;
		}
		break;
	case AIComponent::GRINDER:
		if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->fullyUpgraded()) {
			ai->state = FINDING_PLAYER;
		}
		else {
			ai->state = FINDING_CRYSTAL;
		}
		break;
	case AIComponent::GLASS_CANNON:
		if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->getGunLevel() < 5) {
			ai->state = randomNum(0, 1) ? FINDING_CRYSTAL : FINDING_PLAYER;
		}
		else {
			ai->state = FINDING_PLAYER;
		}
		break;
	case AIComponent::KILL_ONLY:
		ai->state = FINDING_PLAYER;
		break;
	case AIComponent::DEFENSIVE:
		if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->getArmorLevel() < 5) {
			ai->state = FINDING_CRYSTAL;
		}
		else {
			ai->state = FINDING_PLAYER;
		}
		break;	
	}
}

void Logic::reactToAttack(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	switch (ai->getPersonality()) {
	case AIComponent::RANDOM:
		ai->state = randomNum(0, 1) ? FINDING_SAFETY : RETALIATE;
		break;
	case AIComponent::GRINDER:
		if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->fullyUpgraded()) {
			ai->state = RETALIATE;
		}
		else {
			ai->state = FINDING_SAFETY;
		}
		break;
	case AIComponent::GLASS_CANNON:
		if (ai->state == SEEKING_CRYSTAL) {
			ai->state = FINDING_SAFETY;
		}
		else {
			ai->state = RETALIATE;
		}
		break;
	case AIComponent::KILL_ONLY:
		ai->state = RETALIATE;
		break;
	case AIComponent::DEFENSIVE:
		ai->state = RETALIATE;
		break;
	}
}

void Logic::finiteStateMachine(Entity* entity) {
    HealthComponent* aiHealth = static_cast<HealthComponent*>(entity->getComponent(HEALTH));
    if (aiHealth->isDead()) {
        //static_cast<PhysicsComponent*>(entity->getComponent(PHYSICS))->getRigidBody()->setGlobalPose(physx::PxTransform(physx::PxVec3(-350, 10, -350)), false);
        EntityManager::getInstance()->destroyEntity(entity->id);
        return;
    }

	WorldGenerator* world = WorldGenerator::getInstance();
	AStar::Generator* generator = world->getGrid();
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	// at any point, if speed is 0 for set amount of time, back up (unstuckify?)
	checkStuck(entity);

	if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->isUpgradeAvailable()) {
		upgrade(entity);
	}

	// TODO: find closest crystals

	int i;
	switch (ai->state) {
	case DECIDING:
		//std::cout << entity->id << " deciding" << std::endl;
		decide(entity);
		break;
	case FINDING_CRYSTAL:
		//std::cout << entity->id << " searching for crystal path" << std::endl;
		ai->goal = world->getCrystals()->at(randomNum(0, world->getCrystals()->size()-1));
		if (ai->goal != nullptr)
			findPath(generator, entity, ai->goal->getPosition());
		if (ai->path.size() > 0) {
			ai->state = SEEKING_CRYSTAL;
		}
		else {
			ai->state = DECIDING;
		}
		break;
	case SEEKING_CRYSTAL:
		if (ai->getAttackerID() != -1) {
			ai->state = REACT_TO_ATTACK;
			break;
		}
		aiMovement(entity);
		//std::cout << entity->id << " seeking crystal" << std::endl;
		if (ai->currentWaypointIndex == ai->path.size() - 1) {
			ai->state = MINING;
		}
		break;
	case MINING:
		//std::cout << entity->id << " mining crystal" << std::endl;
		mine(entity);
		break;
	case FINDING_PLAYER:
		//std::cout << entity->id << " searching for player path" << std::endl;
		// higher chance of attacking human player
		if (randomNum(0, 10) < 7) {
			i = 0;
		}
		else {
			i = randomNum(0, EntityManager::getInstance()->getVehicleEntities().size() - 1);
		}
		if (EntityManager::getInstance()->getVehicleEntities()[i]->id != entity->id) {
			ai->goal = EntityManager::getInstance()->getVehicleEntities().at(i);
			findPath(generator, entity, ai->goal->getPosition());
			if (ai->path.size() > 0) {
				ai->state = SEEKING_PLAYER;
			}
			else {
				ai->state = DECIDING;
			}
		}
		break;
	case SEEKING_PLAYER:
		if (ai->getAttackerID() != -1) {
			ai->state = REACT_TO_ATTACK;
			break;
		}
		aiMovement(entity);
		//std::cout << entity->id << " seeking player" << std::endl;
		if (ai->currentWaypointIndex == ai->path.size() - 1) {
			ai->state = ATTACKING;
		}
		/*if (glm::distance(ai->goal->getPosition(), entity->getPosition()) < 15.0f) {
			findPath(generator, entity, ai->goal->getPosition());
			ai->state = ATTACKING;
		}*/
		// if goal manages to drive away very fast, give up and do something else
		/*if (glm::distance(ai->goal->getPosition(), entity->getPosition()) > 50.0f) {
			ai->state = DECIDING;
		}*/
		break;
	case ATTACKING:
		if (glm::distance(ai->goal->getPosition(), entity->getPosition()) > 30.0f) {
			findPath(generator, entity, ai->goal->getPosition());
			if (ai->path.size() > 0) {
				ai->state = SEEKING_PLAYER;
			} else { // No path so go back to deciding
                ai->state = DECIDING;
			}
		} else if (glm::distance(ai->goal->getPosition(), entity->getPosition()) > 60.f) {
		    ai->state = DECIDING;
		}
		attack(ai->goal, entity);
		break;
	case STUCK:
		unstuck(entity, generator);
		break;
	case REACT_TO_ATTACK:
		reactToAttack(entity);
		break;
	case RETALIATE:
		//std::cout << entity->id << " retaliating against " << ai->getAttackerID() << std::endl;
		ai->goal = EntityManager::getInstance()->getVehicleEntityWithID(ai->getAttackerID());
        if (ai->goal == nullptr) {
            ai->state = DECIDING;
            ai->setAttackerID(-1);
            break;
        }
		findPath(generator, entity, ai->goal->getPosition());
		if (ai->path.size() > 0) {
			ai->state = SEEKING_PLAYER;
			ai->setAttackerID(-1);
		}
		aiMovement(entity);
		break;
	case FINDING_SAFETY:
		// just go to its opposite position wahahahahaaaa
		ai->goal = EntityManager::getInstance()->getVehicleEntityWithID(ai->getAttackerID());
        if (ai->goal == nullptr) {
            ai->state = DECIDING;
            ai->setAttackerID(-1);
            break;
        }
		findPath(generator, entity, ai->goal->getPosition() * -1.0f);
		//std::cout << entity->id << " fleeing to " << ai->goal->getPosition().x * -1.f << ", " << ai->goal->getPosition().z * -1.f << std::endl;
		if (ai->path.size() > 0) {
			ai->state = FLEEING;
			ai->setAttackerID(-1);
		}
		break;
	case FLEEING:
		if (ai->currentWaypointIndex == ai->path.size() - 1) {
			ai->state = DECIDING;
		}
		aiMovement(entity);
        break;
	}
}

void Logic::bindAudioObservable(AudioObservable* anAudioEventList) {
    audioEvents = anAudioEventList;
}


Logic::~Logic() = default;