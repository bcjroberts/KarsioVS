//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"
#include <glm/gtx/vector_angle.hpp>

Logic::Logic(){
};
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}
void Logic::playerMovement(vehicleInput* newMovement, Entity* targetEnt) {
	DriveComponent* temp = static_cast<DriveComponent*>(targetEnt->getComponent(DRIVE));
	if (temp) temp->setInputs(*newMovement);
}

void Logic::aiMovement(Entity* entity) {
    // First thing we need to do is see if we are close enough to the waypoint to head toward the next one
    AIComponent* ai = static_cast<AIComponent*>(entity->getComponent(AI));

    // Check if the distance is less than some threshold. If so, then we have "arrived at the waypoint", head to the next waypoint.
    if (glm::distance(ai->getCurrentWaypoint(), entity->getPosition()) < 5.0f) {
        ai->nextWaypoint();
    }

    // Now we need to determine where to steer based on where the point is. Currently does not support reverse.
    DriveComponent* aiDrive = static_cast<DriveComponent*>(entity->getComponent(DRIVE));
    bool steerLeft = false, steerRight = false;

    float oangle = glm::orientedAngle(glm::normalize(ai->getCurrentWaypoint() - entity->getPosition()),entity->getForwardVector(), glm::vec3(0,1,0));

    // TODO this should be refined so that the AI doesn't spin out
    if (oangle > 0.1f) {
        steerLeft = true;
    } else if (oangle < -0.1f){
        steerRight = true;
    }

    aiDrive->setInputs(true, false, false, steerLeft, steerRight, false, false);
}

void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}

// rukiya's added stuff
void Logic::findPath(AStar::Generator* generator, Entity* player, Entity* goal) {
	auto path = generator->findPath({vec2(player->getMatrix()[3][0], player->getMatrix()[3][2])}, 
							  {vec2(goal->getMatrix()[3][0], goal->getMatrix()[3][2])});

	for (auto& coordinate : path) {
		//std::cout << coordinate.x << " " << coordinate.y << "\n";
	}
}


Logic::~Logic() = default;