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

private:
	enum States {
		DECIDING,
		FINDING_CRYSTAL,
		FINDING_PLAYER,
		SEEKING_CRYSTAL,
		SEEKING_PLAYER,
		MINING,
		ATTACKING,
		STUCK,
		RETALIATE,
		FLEE,
		RUNNING_AWAY
	};
public:        
    Logic();
    void cameraMovement(Movement* newMovement);
	void playerMovement(Entity* targetEnt);
    void aiMovement(Entity* entity);
    bool canVehicleFlip(Entity* vehicle) const;
    void flipVehicle(Entity* vehicle) const;
    void bindCamera(Camera* aCamera);
	void findPath(AStar::Generator* generator, Entity* entity, glm::vec3 goal);
	//void findPath(AStar::Generator* generator, glm::vec3 start, glm::vec3 goal);
	//void finiteStateMachine(Entity* entity, AStar::Generator* generator, WorldGenerator* world);
	void finiteStateMachine(Entity* entity);
	void mine(Entity* entity);
	void attack(Entity* goal, Entity* entity);
	bool checkStuck(Entity* entity);
	void unstuck(Entity* entity, AStar::Generator* generator);
	int randomNum(int min, int max);
	void upgrade(Entity* entity);
   ~Logic();
};


#endif //RENDERENGINE_LOGIC_H
