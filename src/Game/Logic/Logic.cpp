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
	DriveComponent* temp = static_cast<DriveComponent*>(targetEnt->getComponent(DRIVE));
	if (temp) temp->setInputs(*newMovement);
}
void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}
Logic::~Logic() = default;