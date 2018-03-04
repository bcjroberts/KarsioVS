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

#include "AStar.h"
#include "../../Engine/PhysicsEngine/PhysicsEngine.h"
#include "WorldGenerator.h"
#include "../../Engine/EntityManager.h"

class Logic {
Camera * camera;
Movement * moveCamera;
Entity * player;
vehicleInput * movePlayer;
DriveComponent * playerDrive;
std::vector<vec3> path;
Entity * goal;
int state = 0;
int prevstate = -1;
vec3 prevPos;
int notMoving = 0;
private:
	enum States {
		DECIDING,
		FINDING_CRYSTAL,
		FINDING_PLAYER,
		SEEKING_CRYSTAL,
		SEEKING_PLAYER,
		MINING,
		ATTACKING,
		STUCK
	};
public:        
    Logic();
    void cameraMovement(Movement* newMovement);
	void playerMovement(Entity* targetEnt);
    void aiMovement(Entity* entity);
    bool canVehicleFlip(Entity* vehicle) const;
    void flipVehicle(Entity* vehicle) const;
    void bindCamera(Camera* aCamera);
	void findPath(AStar::Generator* generator, glm::vec3 start, glm::vec3 goal);
	void finiteStateMachine(Entity* entity, AStar::Generator* generator, WorldGenerator* world);
	void mine(Entity* entity);
	void attack(Entity* goal, Entity* entity);
	bool checkStuck(Entity* entity);
	void unstuck(Entity* entity);
   ~Logic();
};


#endif //RENDERENGINE_LOGIC_H
