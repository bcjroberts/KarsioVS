//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"

Logic::Logic(){
};
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}
void Logic::playerMovement(vehicleInput* newMovement, Entity* targetEnt) {
    for (int i = 0; i< targetEnt->myComponents.size(); i++)
    {
        if (targetEnt->myComponents[i]->getComponentType() == DRIVE)
        {
            static_cast<DriveComponent*>(targetEnt->myComponents[i])->setInputs(*newMovement);
        }
    }

}
void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}

// rukiya's added stuff
void Logic::findPath(AStar::Generator* generator, Entity* player, Entity* goal) {
	auto path = generator->findPath({vec2(player->getMatrix()[3][0], player->getMatrix()[3][2])}, 
							  {vec2(goal->getMatrix()[3][0], goal->getMatrix()[3][2])});

	for (auto& coordinate : path) {
		std::cout << coordinate.x << " " << coordinate.y << "\n";
	}
}


Logic::~Logic() = default;