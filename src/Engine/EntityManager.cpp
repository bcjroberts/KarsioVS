

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>
#include "Entity.h"
#include "PxPhysicsAPI.h"
#include "ComponentManager.h"
#include "../Engine/PhysicsEngine/PhysicsEngine.h"
#include "MeshManager.h"

// Initialize the Entity Manager global pointer.
EntityManager *EntityManager::globalInstance = nullptr;

EntityManager::EntityManager() {
}

// Can globally access the EntityManager from anywhere
EntityManager * EntityManager::getInstance()
{
	if (!globalInstance) {
		globalInstance = new EntityManager;
	}
	return globalInstance;
}

EntityManager::~EntityManager() = default;

void EntityManager::destroyEntity(int id) {
	for (uint32_t i = 0; i < entities.size(); ++i) {
		if (entities[i]->id == id) {
			Entity* e = entities[i]; // keep for cleanup purposes
			entities.erase(entities.begin() + i);
			delete e;
			break;
		}
	}
}

Entity* EntityManager::createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
	Entity* e = new Entity(position, rotation, scale);
	entities.push_back(e);
	return e;
}

Entity* EntityManager::createBasicVehicleEntity(glm::vec3 startPos, ShaderData* shaderThisShouldNotBePassedHere) {
    vehicleData* myVehicleData = PhysicsEngine::getInstance()->createVehicle(physx::PxVec3(startPos.x, startPos.y, startPos.z));
    physx::PxShape* shapes[5];
    physx::PxRigidDynamic* rigid1 = myVehicleData->myVehicle->getRigidDynamicActor();
    rigid1->getShapes(shapes, 5);

    // New Entity creation code, place at center of screen, no rotation, scale of 1.
    Entity* entity = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    rigid1->userData = entity;
    //ComponentManager::getInstance()->addRendererComponent(entity1, &cubeMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2.5f, 1.0f, 1.25f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, MeshManager::getMeshData("chassis.obj"), shaderThisShouldNotBePassedHere, shapes[4], glm::vec3(1.0f, 1.0f, 1.0f));
    // Uncomment this if you want to see the physics hitbox
    //ComponentManager::getInstance()->addShapeRendererComponent(entity1, &cubeMesh, &shaderData, shapes[4], glm::vec3(1.5f, 1.0f, 2.5f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, MeshManager::getMeshData("wheels.obj"), shaderThisShouldNotBePassedHere, shapes[0], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.2, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, MeshManager::getMeshData("wheels.obj"), shaderThisShouldNotBePassedHere, shapes[1], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, MeshManager::getMeshData("wheels.obj"), shaderThisShouldNotBePassedHere, shapes[2], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.2, 0, -1.3f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, MeshManager::getMeshData("wheels.obj"), shaderThisShouldNotBePassedHere, shapes[3], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.3f));
    ComponentManager::getInstance()->addPhysicsComponent(entity, rigid1);
    ComponentManager::getInstance()->addDriveComponent(entity, &myVehicleData->myInput);

    return entity;
}

Entity* EntityManager::createGroundPlane(ShaderData* shaderThisShouldNotBePassedHere) {
	Entity* entity = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.0f));
	ComponentManager::getInstance()->addRendererComponent(entity, MeshManager::getMeshData("plane.obj"), shaderThisShouldNotBePassedHere, glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, -1.57f)), glm::vec3(10, 10, 100));
    physx::PxRigidActor* plane = PhysicsEngine::getInstance()->createPhysicsPlane();
    plane->userData = entity;
	ComponentManager::getInstance()->addPhysicsComponent(entity, plane);
	return entity;
}

Entity* EntityManager::createBox(glm::vec3 startPos, glm::vec3 scale, ShaderData* shaderThisShouldNotBePassedHere) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);
	ComponentManager::getInstance()->addRendererComponent(entity, MeshManager::getMeshData("cube.obj"), shaderThisShouldNotBePassedHere, glm::vec3(0), glm::quat(), glm::vec3(1));
    physx::PxRigidActor* box = PhysicsEngine::getInstance()->createPhysicsBox(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(scale));
    box->userData = entity;
    ComponentManager::getInstance()->addPhysicsComponent(entity, box);
	return entity;
}