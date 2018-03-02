//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>
#include "../../Engine/Core.h"
#include <time.h>

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
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	
    // First thing we check is if the Vehicle can flip
    if(canVehicleFlip(entity)) {
        flipVehicle(entity);
    }

	// populate waypoints
	if (ai->waypoints.empty()) {
		ai->setWaypoints(path);
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
	if (ai->currentWaypointIndex == path.size() - 1) {
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
	if (speed > 5 && (steering != 0.0f)) {
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
            //printf("YES\n");
            return true;
        }
    }
    //printf("NO\n");
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


void Logic::findPath(AStar::Generator* generator, glm::vec3 start, glm::vec3 goal) {
	glm::vec3 coarseStart = getCoarsePosition(start);
	glm::vec3 coarseGoal = getCoarsePosition(goal);
	std::vector<vec2> p = generator->findPath({ vec2(coarseStart.x, coarseStart.z) }, { vec2(coarseGoal.x, coarseGoal.z) });
	
	if (p.empty()) {
		std::cout << "no path found" << std::endl;
		return;
	}

	// convert back to vec3 ...
	path.resize(p.size() - 1);
	for (int i = p.size() - 2; i > 0; i--) {	// less current position
		path[i] = vec3(p[i].x, 0, p[i].y);
	}
	// add exact position of goal to the end
	if (path.size() < 1) {
		path.resize(1);
	}
	path[0] = vec3(goal.x, 0, goal.z);
	
	for (int i = path.size() - 1; i > -1; i--) {
		std::cout << path[i].x << " " << path[i].z << std::endl;
	}
}

void Logic::mine(Entity* entity) {
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));

	// start ramming
	if (ai->getMinedID() == ai->getGoalID()) {
		// whether or not it breaks doesn't matter, crystal is hit so it did its job, find something else to do now
		if (ai->getKilledCrystal()) {	// nothing in front of it now

			ai->setMinedID(-2); // reset value
			ai->setKilledCrystal(false);
			state = DECIDING;
		}
		else if (glm::distance(ai->getCurrentWaypoint(), entity->getPosition()) < 20.0f) {
			// crystal is in front of it, back up
			// accel, brake, handbrake, steering
			aiDrive->setInputs(0.0f, 1.0f, 0.0f, 0.0f);
		}
		else {
			// backed away far enough
			state = DECIDING;
		}
	}
	else if (ai->getMinedID() != ai->getGoalID() && ai->getKilledCrystal()) {
		ai->setKilledCrystal(false);	// got the wrong crystal, keep going
	}
	else {
		aiMovement(entity);
	}
}

void Logic::attack(Entity* goal, Entity* entity) {
	// shooting?
	aiMovement(entity);
}

void Logic::finiteStateMachine(Entity* entity, AStar::Generator* generator, WorldGenerator* world) {
	EntityManager* entityMan;
	AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	
	int decision;
	switch (state) {
	case DECIDING:
		ai->clearWaypoints();
		srand(time(NULL));
		decision = rand() % 2;
		if (decision == 0) {
			state = FINDING_CRYSTAL;
		}
		else if (decision == 1) {
			state = FINDING_PLAYER;
		}
		break;
	case FINDING_CRYSTAL:
		srand(time(NULL));
		do {
			goal = world->getCrystals()->at(rand() % world->getCrystalSize());
			ai->setGoalID(goal->id);
			// atm, only look for small crystals
		} while (goal->getScale().x > 1.5);	// assuming scale is same in x, y, z?
		findPath(generator, entity->getPosition(), goal->getPosition());
		if (path.size() > 0) {
			state = SEEKING_CRYSTAL;
		}
		break;
	case SEEKING_CRYSTAL:
		aiMovement(entity);
		if (ai->currentWaypointIndex == path.size() - 1) {
			state = MINING;
		}
		break;
	case MINING:
		mine(entity);
		break;
	case FINDING_PLAYER:
		goal = entityMan->getInstance()->getEntities().at(0);
		findPath(generator, entity->getPosition(), goal->getPosition());
		if (path.size() > 0) {
			state = SEEKING_PLAYER;
		}
		break;
	case SEEKING_PLAYER:
		aiMovement(entity);
		if (ai->currentWaypointIndex == path.size() - 1) {
			state = ATTACKING;
		}
		break;
	case ATTACKING:
		attack(goal, entity);
		if (glm::distance(goal->getPosition(), entity->getPosition()) > 30.0f) {
			// do something else if they get too far away
			state = DECIDING;
		}
		break;
	}
}




Logic::~Logic() = default;