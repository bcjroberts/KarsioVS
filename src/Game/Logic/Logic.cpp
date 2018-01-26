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
Logic::~Logic() = default;