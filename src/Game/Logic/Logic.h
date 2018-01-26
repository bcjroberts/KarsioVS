//
// Created by Sebastian on 12/23/2017.
//
#pragma once
#ifndef RENDERENGINE_LOGIC_H
#define RENDERENGINE_LOGIC_H
#include "../../Engine/Entity.h"
#include "../../Engine/ComponentManager.h"
#include "../Components/DriveComponent.h"
#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include "../../Main/initOpenGL/OpenGL.h"
#include "../Camera.h"

class Logic {
Camera * camera;
Movement * moveCamera;
Entity * player;
vehicleInput * movePlayer;
DriveComponent * playerDrive;
public:        
    Logic();
    void updateInputs();
    void cameraMovement(Movement* newMovement);
    void playerMovement(vehicleInput* playerInput);
    void bindCamera(Camera* aCamera);
    void bindPlayer(Entity* targetEnt);
   ~Logic();
};


#endif //RENDERENGINE_LOGIC_H
