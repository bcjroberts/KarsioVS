//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>
#include "../../Engine/Core.h"
#include <random>
#include <chrono> 

Logic::Logic(){
};
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}

//void Logic::playerMovement(vehicleInput *newMovement, Entity* targetEnt) {
void Logic::playerMovement(Entity* targetEnt) {
	ControllableComponent* con = static_cast<ControllableComponent*>(targetEnt->getComponent(CONTROLLABLE));
	DriveComponent* temp = static_cast<DriveComponent*>(targetEnt->getComponent(DRIVE));
	//if (temp) temp->setInputs(*newMovement);
	con->getInput();
	temp->setInputs(con->input);

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
    Core::renderEngine->ui->modifyText(2, &chassisLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Armor lvl: " << uc->getArmorLevel();
    std::string armorLevel = oss.str();
    Core::renderEngine->ui->modifyText(3, &armorLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Gun lvl: " << uc->getGunLevel();
    std::string gunLevel = oss.str();
    Core::renderEngine->ui->modifyText(4, &gunLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    oss << "Ram lvl: " << uc->getRamLevel();
    std::string ramLevel = oss.str();
    Core::renderEngine->ui->modifyText(5, &ramLevel, nullptr, nullptr, nullptr, nullptr);
    oss.str("");
    oss.clear();

    // Now check for upgrades
    glm::vec3 upgradeColor(0,1,0);
    glm::vec3 defaultColor(1,1,0);
    std::string empty = "";
    if (uc->canUpgradeType(CHASSIS_UPGRADE)) {
        std::string chassisUpgrade = "UP: Press 3 or DPAD Up";
        Core::renderEngine->ui->modifyText(6, &chassisUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeChassis) {
            uc->upgradeVehicle(CHASSIS_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(6, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(ARMOR_UPGRADE)) {
        std::string armorUpgrade = "UP: Press 4 or DPAD Right";
        Core::renderEngine->ui->modifyText(7, &armorUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeArmor) {
            uc->upgradeVehicle(ARMOR_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(7, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(GUN_UPGRADE)) {
        std::string gunUpgrade = "UP: Press 5 or DPAD Down";
        Core::renderEngine->ui->modifyText(8, &gunUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeGun) {
            uc->upgradeVehicle(GUN_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(8, &empty, nullptr, nullptr, nullptr, nullptr);
    }
    if (uc->canUpgradeType(RAM_UPGRADE)) {
        std::string ramUpgrade = "UP: Press 6 or DPAD Left";
        Core::renderEngine->ui->modifyText(9, &ramUpgrade, nullptr, nullptr, nullptr, nullptr);
        if(con->upInput.upgradeRam) {
            uc->upgradeVehicle(RAM_UPGRADE);
        }
    } else {
        Core::renderEngine->ui->modifyText(9, &empty, nullptr, nullptr, nullptr, nullptr);
    }
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	// populate waypoints
	if (ai->waypoints.empty()) {
		ai->setWaypoints(ai->path);
	}
	
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
	// accel, brake, handbrake, steering
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
        if (Core::timeSinceStartup - static_cast<DriveComponent*>(vehicle->getComponent(DRIVE))->previousFlipTime > 2.5f) {
            return true;
        }
    }
    
    return false;
}

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

    const physx::PxVec3 verticalForce = physx::PxVec3(0, 1, 0) * 10000.f;
    const physx::PxVec3 torqueForce = PhysicsEngine::toPxVec3(torqueDirection) * 5000.f;

    static_cast<PhysicsComponent*>(vehicle->getComponent(PHYSICS))->getRigidBody()->addForce(verticalForce, physx::PxForceMode::eIMPULSE, true);
    static_cast<PhysicsComponent*>(vehicle->getComponent(PHYSICS))->getRigidBody()->addTorque(torqueForce, physx::PxForceMode::eIMPULSE, true);

    // Set the time since the last flip attempt was made.
    static_cast<DriveComponent*>(vehicle->getComponent(DRIVE))->previousFlipTime = Core::timeSinceStartup;
}

void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}

glm::vec3 getCoarsePosition(glm::vec3 position) {
	float x = position.x;
	float y = position.y;
	float z = position.z;
	float gridSize = 10; // larger size = larger grid

	x = floor(x / gridSize);
	y = floor(y / gridSize);
	z = floor(z / gridSize);
	return glm::vec3(x, y, z);
}


void Logic::findPath(AStar::Generator* generator, Entity* entity, glm::vec3 goal) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	
	glm::vec3 coarseStart = getCoarsePosition(entity->getPosition());
	glm::vec3 coarseGoal = getCoarsePosition(goal);
	std::vector<vec2> p = generator->findPath({ vec2(coarseStart.x, coarseStart.z) }, { vec2(coarseGoal.x, coarseGoal.z) });
	
	if (p.empty()) {
		//std::cout << "no path found" << std::endl;
		ai->path.resize(0);
		return;
	}

	// convert back to vec3 ...
	ai->path.resize(p.size() - 1);
	for (int i = p.size() - 2; i > 0; i--) {	// less current position
		ai->path[i] = vec3(p[i].x, 0, p[i].y);
	}
	// add exact position of goal to the end
	if (ai->path.size() < 1) {
		ai->path.resize(1);
	}
	ai->path[0] = vec3(goal.x, 0, goal.z);
	std::cout << "path for " << entity->id << std::endl;
	for (int i = ai->path.size() - 1; i > -1; i--) {
		std::cout << ai->path[i].x << " " << ai->path[i].z << std::endl;
	}	
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
	// start ramming
	if (ai->getMinedID() == ai->goalID) {
		// whether or not it breaks doesn't matter, crystal is hit so it did its job, find something else to do now
		if (ai->getKilledCrystal()) {	// nothing in front of it now

			ai->setMinedID(-2); // reset value
			ai->setKilledCrystal(false);
			ai->state = DECIDING;
		}
		else if (glm::distance(ai->getCurrentWaypoint(), entity->getPosition()) < 7.0f) {
			// crystal is in front of it, back up
			// accel, brake, handbrake, steering
			aiDrive->setInputs(0.0f, 1.0f, 0.0f, 0.0f);
		}
		else {
			// backed away far enough
			ai->state = DECIDING;
		}
	}
	else if (ai->getMinedID() != ai->goalID && ai->getKilledCrystal()) {
		ai->setKilledCrystal(false);	// got the wrong crystal, keep going
	}
	else {
		aiMovement(entity);
	}
}

void Logic::attack(Entity* goal, Entity* entity) {
    // Determine if we should shoot by seeing if we are almost facing the direction of the player.
    float oangle = glm::orientedAngle(glm::normalize(goal->getPosition() - entity->getPosition()),entity->getForwardVector(), glm::vec3(0,1,0));
    if (abs(oangle) < 0.5f) {
        static_cast<WeaponComponent*>(entity->getComponent(WEAPON))->fireWeapon();
    }

	aiMovement(entity);
}

bool Logic::checkStuck(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(entity->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());
	float speed = glm::dot(velocity, entity->getForwardVector());

    // First thing we check is if the Vehicle can flip
    if(canVehicleFlip(entity)) {
        flipVehicle(entity);
    }

	//std::cout << "speed = " << speed << std::endl;
	if (speed < 1 && speed > -1) {
		ai->notMoving++;
	}
	else {
		ai->notMoving = 0;
	}
	// speed has to be 0 for a while
	if (ai->notMoving > 70 && ai->state != STUCK) {
		if (ai->prevstate == -1) {
			ai->prevstate = ai->state;
		}
		ai->state = STUCK;
		ai->prevPos = entity->getPosition();
		return true;
	}
	else {
		return false;
	}
}

void Logic::unstuck(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));

	if (glm::distance(ai->prevPos, entity->getPosition()) < 7.0f) {
		aiDrive->setInputs(0.0f, 1.0f, 0.0f, 0.0f);
	}
	else {
		// continue with whatever it was doing
		ai->state = ai->prevstate;
		ai->notMoving = 0;
	}
}

int randomNum(int min, int max) {
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> uid{ min, max };
	return uid(dre);
}

void Logic::upgrade(Entity* entity) {
	// just randomly upgrade anything right now
	UpgradeType type = UpgradeType(randomNum(0, 3));
	while (!static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->canUpgradeType(type)) {
		type = UpgradeType(randomNum(0, 3));
	}
	static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->upgradeVehicle(type);
}

void Logic::finiteStateMachine(Entity* entity, AStar::Generator* generator, WorldGenerator* world) {

    HealthComponent* aiHealth = static_cast<HealthComponent*>(entity->getComponent(HEALTH));
    if (aiHealth->isDead()) {
        if (aiHealth->isDeathProcessed()) {
            static_cast<PhysicsComponent*>(entity->getComponent(PHYSICS))->getRigidBody()->setGlobalPose(physx::PxTransform(physx::PxVec3(-150, 10, -150)), false);
        }
        return;
    }

	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

	// at any point, if speed is 0 for set amount of time, back up (unstuckify?)
	checkStuck(entity);
	// at any point, if upgrade available, upgrade
	if (static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE))->isUpgradeAvailable()) {
		upgrade(entity);
	}
	
	int decision;
	switch (ai->state) {
	case DECIDING:
		ai->clearWaypoints();
		decision = randomNum(0, 1);
		if (decision == 0) {
			ai->state = FINDING_CRYSTAL;
		}
		else if (decision == 1) {
			ai->state = FINDING_PLAYER;
		}
		break;
	case FINDING_CRYSTAL:
		do {
			ai->goal = world->getCrystals()->at(randomNum(0, world->getCrystalSize()-1));
			// atm, only look for small crystals
		} while (ai->goal->getScale().x > 1.5);	// assuming scale is same in x, y, z?
		std::cout << "searching for crystal path" << std::endl;
		findPath(generator, entity, ai->goal->getPosition());
		if (ai->path.size() > 0) {
			ai->state = SEEKING_CRYSTAL;
		}
		break;
	case SEEKING_CRYSTAL:
		aiMovement(entity);
		if (ai->currentWaypointIndex == ai->path.size() - 1) {
			ai->state = MINING;
		}
		break;
	case MINING:
		mine(entity);
		break;
	case FINDING_PLAYER:
		int i;
		do {
			i = randomNum(0, 2);
			//i = randomNum(0, EntityManager::getInstance()->getEntities().size() - 1);
		} while (EntityManager::getInstance()->getEntities().at(i)->id == entity->id);
		ai->goal = EntityManager::getInstance()->getEntities().at(i);
		std::cout << "searching for player path" << std::endl;
		findPath(generator, entity, ai->goal->getPosition());
		if (ai->path.size() > 0) {
			std::cout << "seeking player id " << ai->goal->id << ", my id is " << entity->id << std::endl;
			ai->state = SEEKING_PLAYER;
		}
		break;
	case SEEKING_PLAYER:
		aiMovement(entity);
		if (ai->currentWaypointIndex == ai->path.size() - 1) {
			ai->state = ATTACKING;
		}
		break;
	case ATTACKING:
		attack(ai->goal, entity);
		if (glm::distance(ai->goal->getPosition(), entity->getPosition()) > 30.0f) {
			// do something else if they get too far away
			ai->state = DECIDING;
		}
		break;
	case STUCK:
		unstuck(entity);
		break;
	}
}




Logic::~Logic() = default;