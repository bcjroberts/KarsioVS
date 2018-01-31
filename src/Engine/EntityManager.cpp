

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>
#include "Entity.h"
#include "PxPhysicsAPI.h"
#include "ComponentManager.h"
#include "../Engine/PhysicsEngine/PhysicsEngine.h"

// Initialize the Entity Manager global pointer.
EntityManager *EntityManager::globalInstance = nullptr;

MeshData wheelMesh("wheelMesh");
MeshData chassisMesh("chassisMesh");


EntityManager::EntityManager() {
    wheelMesh.loadMeshData("data/assets/meshes/wheels.obj");
    chassisMesh.loadMeshData("data/assets/meshes/chassis.obj");
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

Entity* EntityManager::createBasicVehicleEntity(ShaderData* shaderThisShouldNotBePassedHere) {
    vehicleData* myVehicleData = PhysicsEngine::getInstance()->createVehicle(physx::PxVec3(0, 0, 0));
    physx::PxShape* shapes[5];
    physx::PxRigidDynamic* rigid1 = myVehicleData->myVehicle->getRigidDynamicActor();
    rigid1->getShapes(shapes, 5);

    // New Entity creation code, place at center of screen, no rotation, scale of 1.
    Entity* entity1 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    //ComponentManager::getInstance()->addRendererComponent(entity1, &cubeMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2.5f, 1.0f, 1.25f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &chassisMesh, shaderThisShouldNotBePassedHere, shapes[4], glm::vec3(1.0f, 1.0f, 1.0f));
    // Uncomment this if you want to see the physics hitbox
    //ComponentManager::getInstance()->addShapeRendererComponent(entity1, &cubeMesh, &shaderData, shapes[4], glm::vec3(1.5f, 1.0f, 2.5f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, shaderThisShouldNotBePassedHere, shapes[0], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.2, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, shaderThisShouldNotBePassedHere, shapes[1], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, shaderThisShouldNotBePassedHere, shapes[2], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.2, 0, -1.3f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, shaderThisShouldNotBePassedHere, shapes[3], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.3f));
    ComponentManager::getInstance()->addPhysicsComponent(entity1, rigid1);
    ComponentManager::getInstance()->addDriveComponent(entity1, &myVehicleData->myInput);

    return entity1;
}
