//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>
#include "../../Engine/Core.h"
#include <random>
#include <chrono>
#include "../../Engine/AudioPaths.h"
#include "../../Engine/Importer/Managers/TextureDataManager.h"

Logic::Logic(){
};
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}

int upgradeText = -1;

void Logic::displayUpgradeUI(UpgradeComponent* upgradeComp) {

	// If an upgrade is available, display the upgrade text along the top.
	// Otherwise return
	float scale = 0;
	if (!upgradeComp->isUpgradeAvailable()) {
		if (upgradeText != -1) {
			Core::renderEngine->ui->removeText(upgradeText);
			upgradeText = -1;
			//Core::renderEngine->ui->removeImage(currentImageUiIds[0]);
			//currentImageUiIds.clear();
			scale = 0;
			Core::renderEngine->ui->modifyImage(Core::upgradeLizardId, nullptr, nullptr, &scale);
		}
		return;
	} else if (upgradeText == -1) {
		scale = 1;
		Core::renderEngine->ui->modifyImage(Core::upgradeLizardId, nullptr, nullptr, &scale);
		//upgradeText = Core::renderEngine->ui->addText("Upgrade Available", 1610, 557, 0.40f, glm::vec3(0.8, 1, 1));
		upgradeText = 1;
		//upgradeText = Core::renderEngine->ui->addText("Upgrade Available: Press X to Choose", 350, 20, 1.4f, glm::vec3(1, 1, 0));
	}
}

//void Logic::playerMovement(vehicleInput *newMovement, Entity* targetEnt) {
void Logic::playerMovement(Entity* targetEnt) {
	ControllableComponent* con = static_cast<ControllableComponent*>(targetEnt->getComponent(CONTROLLABLE));
	DriveComponent* playerDrive = static_cast<DriveComponent*>(targetEnt->getComponent(DRIVE));
    HealthComponent* hc = static_cast<HealthComponent*>(targetEnt->getComponent(HEALTH));
	//if (temp) temp->setInputs(*newMovement);
    if (!hc->isDead()) {
	    con->getInput();
    	playerDrive->setInputs(con->input);
    } else {
        playerDrive->setInputs(0, 0, 0, 0, false, false);
        return;
    }

    if (playerDrive->getFlip() && canVehicleFlip(targetEnt)) {
        flipVehicle(targetEnt);
    }

    if (playerDrive->getShooting()) {
        static_cast<WeaponComponent*>(targetEnt->getComponent(WEAPON))->fireWeapon();
    }


    UpgradeComponent* uc = static_cast<UpgradeComponent*>(targetEnt->getComponent(UPGRADE));
	displayUpgradeUI(uc);
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

    // Check if the distance is less than some threshold. If so, then we have "arrived at the waypoint", head to the next waypoint.
    if (glm::distance(ai->getCurrentWaypoint(), entity->getCoarsePosition()) < 1.0f) {
        ai->nextWaypoint();
        ai->orbitingSimTime = Core::simtimeSinceStartup;
    }
	
    glm::vec3 destination = ai->getCurrentWaypoint();

    // Perform a LOS check every 0.5 seconds
    if (ai->goal != nullptr) {
        if (Core::simtimeSinceStartup - ai->lastLOSCheckTime > 0.1f) {
            if (isGoalInLOS(entity, ai->goal)) {
                ai->successfulLOSChecks++;
                ai->isDestinationInLOS = ai->successfulLOSChecks > 5;
            } else {
                ai->successfulLOSChecks = 0;
                ai->isDestinationInLOS = false;
            }
            ai->lastLOSCheckTime = Core::simtimeSinceStartup;
        }

        // If we can see the target or it is really close, just try to go to it.
        if (ai->isDestinationInLOS || glm::distance(entity->getPosition(), ai->goal->getPosition()) < 15.f) {
            destination = ai->goal->getPosition();

            // If we can go straight to our target, get the last waypoint in the waypoints array
            while (!ai->path.empty() && ai->currentWaypointIndex < ai->path.size() - 1) {
                ai->nextWaypoint();
            }
        }
    } else {
        ai->successfulLOSChecks = 0;
        ai->isDestinationInLOS = false;
    }

    if (Core::simtimeSinceStartup - ai->orbitingSimTime > 5.0f && !ai->isDestinationInLOS) { // We are orbitting the waypoint, go decide what to so
        ai->state = DECIDING;
        ai->orbitingSimTime = Core::simtimeSinceStartup;
    }

    // Now we need to determine where to steer based on where the point is. Currently does not support reverse.
    DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
    float steering = 0.0f;
    float accel = 0.0f;
    float oangle = glm::orientedAngle(glm::normalize(destination - entity->getCoarsePosition()),entity->getForwardVector(), glm::vec3(0,1,0));
	
	// if at last waypoint, move directly towards goal
	if (ai->currentWaypointIndex == ai->path.size() - 1) {
		oangle = glm::orientedAngle(glm::normalize(destination - entity->getPosition()), entity->getForwardVector(), glm::vec3(0, 1, 0));
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
        accel = ai->isDestinationInLOS ? 1.0f : 0.8f;
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
        if (Core::simtimeSinceStartup - static_cast<DriveComponent*>(vehicle->getComponent(DRIVE))->previousFlipTime > 1.75f) {
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
	
	// Give the car 5 seconds to collect the crystal
	if (Core::simtimeSinceStartup -  ai->orbitingSimTime > 5.0f) {
		ai->state = DECIDING;
	}
	if (ai->getMinedID() == ai->goal->id) {
		// if it hit crystal and didn't break it, probably won't be able to break it anyways
		ai->state = DECIDING;
		ai->setMinedID(-2);
	}
	else {
		aiMovement(entity);
	}
}

void Logic::attack(Entity* goal, Entity* entity) {
	std::vector<Entity*> e = EntityManager::getInstance()->getEntities();

	// check if entity is still alive
	if ((std::find(e.begin(), e.end(), static_cast<AIComponent*>(entity->getComponent(AI))->goal) == e.end())) {
		static_cast<AIComponent*>(entity->getComponent(AI))->state = DECIDING;
		return;
	}

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

	if (abs(magnitude) > 1.f) {
		ai->notMoving = Core::simtimeSinceStartup;
	}
	// speed has to be 0 for a while
	if (Core::simtimeSinceStartup - ai->notMoving > 2.0f && ai->state != STUCK) {
		if (ai->prevstate == -1) {
			ai->prevstate = ai->state;
		}
		ai->state = STUCK;
		ai->prevPos = entity->getPosition();
        ai->stuckStartSimTime = Core::simtimeSinceStartup;
		return true;
	} else {
		return false;
	}
}

const float backwardsTime[3] = {0.75f, 1.2f, 2.f};

void Logic::unstuck(Entity* entity, AStar::Generator* generator) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
	UpgradeComponent* uc = static_cast<UpgradeComponent*>(entity->getComponent(UPGRADE));

    // Drive backwards for 1 second
	if (Core::simtimeSinceStartup - ai->stuckStartSimTime < backwardsTime[uc->getChassisLevel() - 1]) {
		aiDrive->setInputs(0.0f, 1.0f, 0.0f, 0.0f);
	}
	else {
		if (ai->goal != nullptr) {
			findPath(generator, entity, ai->goal->getPosition());
			if (ai->path.size() > 0) {
				ai->state = ai->prevstate;
                ai->notMoving = Core::simtimeSinceStartup;
			}
		} else {
			ai->state = DECIDING;
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
			ai->state = randomNum(0, 1) ? FINDING_CRYSTAL : FINDING_PLAYER;
		}
		else {
			ai->state = FINDING_PLAYER;
		}
		break;	
	}
}


const float raycastOffsets[3] = {4.f, 5.f, 7.f};

// Checks to see if our goal is within our LOS. If so, we can just drive towards it and skip the rest of the waypoints.
bool Logic::isGoalInLOS (Entity* originEnt, Entity* destEnt) {
    
    float dist = glm::distance(originEnt->getPosition(), destEnt->getPosition());
    // Don't perform the check unless the AI is close enough to the target
    if (dist > 100.f) return false;

    float originOffset = raycastOffsets[static_cast<UpgradeComponent*>(originEnt->getComponent(UPGRADE))->getChassisLevel() - 1];
    physx::PxRaycastBuffer* result = new physx::PxRaycastBuffer();
    physx::PxVec3 dir = PhysicsEngine::toPxVec3(glm::normalize(destEnt->getPosition() - originEnt->getPosition()));
    physx::PxVec3 origin =  physx::PxVec3(originEnt->getPosition().x + (dir.x * originOffset), 
        originEnt->getPosition().y + (dir.y * originOffset), originEnt->getPosition().z + (dir.z * originOffset));
    
    if (PhysicsEngine::getInstance()->fireRaycast(result, origin, dir, dist)) {
        // Now we need to check to see if what we hit was infact the target entity
        if (result->block.actor->userData == destEnt) {
            return true;
        }
    } else { // We did not hit anything, so we must be able to see our destination.
        return true;
    }

    return false;
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

	int i;
	switch (ai->state) {
	case DECIDING:
		//std::cout << entity->id << " deciding" << std::endl;
		ai->goal = nullptr;
		decide(entity);
		break;
	case FINDING_CRYSTAL:
		//std::cout << entity->id << " searching for crystal path" << std::endl;
		if (world->getCrystals()->empty()) {
			ai->state = FINDING_PLAYER;
			break;
		}
		ai->goal = world->getCrystals()->at(randomNum(0, world->getCrystals()->size() - 1));
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
		if (ai->getAttackerID() == 0) {
			ai->state = RETALIATE;
			break;
		}
		aiMovement(entity);
		//std::cout << entity->id << " seeking crystal" << std::endl;
		if (ai->currentWaypointIndex == ai->path.size() - 1 || ai->isDestinationInLOS) {
			ai->state = MINING;
			ai->orbitingSimTime = Core::simtimeSinceStartup;
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
			if (!ai->path.empty()) {
				ai->state = SEEKING_PLAYER;
			}
			else {
				ai->state = DECIDING;
			}
		}
		break;
	case SEEKING_PLAYER:
		if (ai->getAttackerID() == 0) {
			ai->state = RETALIATE;
			break;
		}
		aiMovement(entity);
		if (ai->currentWaypointIndex == ai->path.size() - 1 || ai->isDestinationInLOS) {
			ai->state = ATTACKING;
		}

		break;
	case ATTACKING:
		if (!ai->isDestinationInLOS) {
			findPath(generator, entity, ai->goal->getPosition());
			if (ai->path.size() > 0) {
				ai->state = SEEKING_PLAYER;
			}
			else { // No path so go back to deciding
				ai->state = DECIDING;
			}
		}
		else if (glm::distance(ai->goal->getPosition(), entity->getPosition()) > 175.f) { // If the target gets further away than we can detect, stop attacking.
			ai->state = DECIDING;
		}
		attack(ai->goal, entity);
		break;
	case STUCK:
		unstuck(entity, generator);
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
	}
}

void Logic::bindAudioObservable(AudioObservable* anAudioEventList) {
    audioEvents = anAudioEventList;
}


Logic::~Logic() = default;