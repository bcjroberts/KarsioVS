//
// Created by Sebastian on 12/23/2017.
//

#include "Logic.h"

Logic::Logic(){
};
void Logic::updateInputs() {
    camera->moveCamera(*moveCamera, 0.5f);
    playerDrive->setInputs(*movePlayer);
}
void Logic::cameraMovement(Movement* newMovement) {
    moveCamera = newMovement;
}
void Logic::playerMovement(vehicleInput* newMovement) {
    movePlayer = newMovement;
}
void Logic::bindCamera(Camera* aCamera) {
    camera = aCamera;
}
void Logic::bindPlayer(Entity* targetEnt) {
    player = targetEnt;
    for (int i=0; i< player->myComponents.size(); i++) 
    {
        if (player->myComponents[i]->getComponentType() == DRIVE) {
            playerDrive = (DriveComponent*)player->myComponents[i];
        }
    }
}



Logic::~Logic() = default;