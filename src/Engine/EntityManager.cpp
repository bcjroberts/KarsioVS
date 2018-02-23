

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>
#include "Entity.h"
#include "PxPhysicsAPI.h"
#include "ComponentManager.h"
#include "../Engine/PhysicsEngine/PhysicsEngine.h"
//#include "Importer/Managers/MeshManager.h"
#include "Importer/Managers/ModelManager.h"

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
            ComponentManager::getInstance()->cleanupComponents(e);
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

Entity* EntityManager::createBasicVehicleEntity(glm::vec3 startPos) {
    vehicleData* myVehicleData = PhysicsEngine::getInstance()->createVehicle(physx::PxVec3(startPos.x, startPos.y, startPos.z));
    physx::PxShape* shapes[6];
    physx::PxRigidDynamic* rigid1 = myVehicleData->myVehicle->getRigidDynamicActor();
    rigid1->getShapes(shapes, 6);

    // New Entity creation code, place at center of screen, no rotation, scale of 1.
    Entity* entity = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    rigid1->userData = entity;
    //ComponentManager::getInstance()->addRendererComponent(entity1, &cubeMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2.5f, 1.0f, 1.25f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("chassis-lvl1"), shapes[4], glm::vec3(1.0f, 1.0f, 1.0f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("gunHolder-lvl1"), shapes[5], glm::vec3(1.f), glm::vec3(0.f, 1.f, -3.f));
	//ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModelData("ram5"), shapes[5], glm::vec3(1.f), glm::vec3(0, -0.9f, -0.7f));

    // Uncomment this if you want to see the physics hitbox for the chassis
    //ComponentManager::getInstance()->addShapeRendererComponent(entity1, &cubeMesh, &shaderData, shapes[4], glm::vec3(1.5f, 1.0f, 2.5f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[0], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.4, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[1], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.6f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[2], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.4, 0, -1.3f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[3], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.3f));
    ComponentManager::getInstance()->addPhysicsComponent(entity, rigid1);
    ComponentManager::getInstance()->addDriveComponent(entity, &myVehicleData->myInput);
    ComponentManager::getInstance()->addHealthComponent(entity, 200.f);

    return entity;
}

Entity* EntityManager::createPlayerVehicleEntity(glm::vec3 startPos) {
	Entity* playerCar = createBasicVehicleEntity(startPos);

	ComponentManager::getInstance()->addControllableComponent(playerCar, true);
	
	return playerCar;
}

Entity* EntityManager::createAIVehicleEntity(glm::vec3 startPos) {
    Entity* aiCar = createBasicVehicleEntity(startPos);

    // We now have a regular car, lets AI-ify it.
    ComponentManager::getInstance()->addAIComponent(aiCar);
	ComponentManager::getInstance()->addControllableComponent(aiCar, false);
    return aiCar;
}

Entity* EntityManager::createGroundPlane() {
	Entity* entity = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.0f));
	ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("plane"), glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, -1.57f)), glm::vec3(100, 10, 100));
    physx::PxRigidActor* plane = PhysicsEngine::getInstance()->createPhysicsPlane();
    plane->userData = entity;
	ComponentManager::getInstance()->addPhysicsComponent(entity, plane);
	return entity;
}

Entity* EntityManager::createBox(glm::vec3 startPos, glm::vec3 scale) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);
	ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), glm::vec3(1));
    physx::PxRigidActor* box = PhysicsEngine::getInstance()->createPhysicsBox(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(scale));
    box->userData = entity;
    ComponentManager::getInstance()->addPhysicsComponent(entity, box);
	return entity;
}

Entity* EntityManager::createCrystal(glm::vec3 startPos, float resourceAmount) {
    if (resourceAmount <= 0.0f || resourceAmount > 2.5f) { // if the value is not within 0.5 and 2.5, generate it.
        resourceAmount = float(rand() % 2000 + 500) / 1000.0f;
    }

    glm::vec3 physicsScale = glm::vec3(1.0f, 4.0f, 1.0f) * resourceAmount;
    glm::vec3 modelScale = glm::vec3(1.0f) * resourceAmount;
    Model* crystalModel = nullptr;
    float heightOffset = 0;
    if (resourceAmount <= 1.5f) {
        crystalModel = ModelManager::getModel("smallCrystal1");
        heightOffset = (physicsScale.y - (modelScale.y + resourceAmount * 2.75f) + 4.0f) / 2.0f;
    } else {
        modelScale *= 0.5f;
        physicsScale *= 1.0f;
        crystalModel = ModelManager::getModel("bigCrystal1");
        heightOffset = (physicsScale.y - (modelScale.y + resourceAmount * 3.5f) + 8.0f) / 2.0f;
    }

    
    startPos.y = resourceAmount * 2.0f;
    Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), glm::vec3(1));
   // ComponentManager::getInstance()->addRendererComponent(entity, crystalModel, glm::vec3(0,-heightOffset,0), glm::quat(glm::vec3(0, float(rand() % 314) / 100.0f,0)), modelScale);
    ComponentManager::getInstance()->addRendererComponent(entity, crystalModel, glm::vec3(0,-heightOffset,0), glm::quat(glm::vec3(-1.57,0,0)), modelScale);

	ComponentManager::getInstance()->addHealthComponent(entity, resourceAmount * 100.f, true);
    
    // Render the physics hitbox for the crystal
    // ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), physicsScale);

    physx::PxRigidActor* box = PhysicsEngine::getInstance()->createCrystalBoxCollider(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(physicsScale));
    box->userData = entity;
    ComponentManager::getInstance()->addPhysicsComponent(entity, box);
    return entity;
}