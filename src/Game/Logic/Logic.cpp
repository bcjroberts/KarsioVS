//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>
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
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));
	
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
		
		// if reached it, do something else (change this later)
		if (glm::distance(ai->getCurrentWaypoint(), entity->getPosition()) < 0.5f) {
			state = 2;
			std::cout << "arrived at goal; find out what to do next; state = 2" << std::endl;
			ai->clearWaypoints();
		}
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
	//std::cout << "goal coarse " << coarseGoal.x << " " << coarseGoal.z << std::endl;
	if (p.empty()) {
		std::cout << "no path found; search for new goal; state = 0" << std::endl;
		state = 0;
		return;
	}

	// convert back to vec3 ...
	path.resize(p.size() - 1);
	for (int i = p.size() - 2; i > 0; i--) {	// less current position
		path[i] = vec3(p[i].x, 0, p[i].y);
	}
	// add exact position of goal to the end
	path[0] = vec3(goal.x, 0, goal.z);
	state = 1;
	std::cout << "found path to goal; state = 1" << std::endl;
	std::cout << "current position " << coarseStart.x << " " << coarseStart.z << std::endl;
	for (int i = path.size() - 1; i > -1; i--) {
		std::cout << path[i].x << " " << path[i].z << std::endl;
	}
}

// states
// 0 = searching for goal location, getting path to goal
// 1 = moving towards goal
// 2 = arrived at goal (branch depending on what the goal is)
// 3 = upgrading
void Logic::stateThing(Entity* entity, AStar::Generator* generator, WorldGenerator* world) {
	Entity* goal;
	switch (state) {
	case 0:
		srand(time(NULL));
		goal = world->getCrystals()->at(rand() % world->getCrystalSize());
		findPath(generator, entity->getPosition(), goal->getPosition());
		break;
	case 1:
		aiMovement(entity);
		break;
	case 2:
		// figure out what to do next based on behaviour
		// if upgrade possible, upgrade (and change to state 3)
		// else, search for crystal (change to state 0)
		// attack player
		goal = 
		break;
	case 3:
		// upgrade based on behaviour
		break;
	}
}




Logic::~Logic() = default;