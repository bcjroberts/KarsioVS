

#include "EntityManager.h"
#include <glm/gtc/quaternion.hpp>
#include "Entity.h"
#include "PxPhysicsAPI.h"
#include "ComponentManager.h"
#include "../Engine/PhysicsEngine/PhysicsEngine.h"
#include "Importer/Managers/ModelManager.h"
#include "Core.h"

#include <random>
#include <chrono> 
#include "../Game/Components/FloatingTextComponent.h"
#include <glm/gtx/quaternion.hpp>

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

void EntityManager::bindAudioObservable(AudioObservable* anEventList) {
    audioEvents = anEventList;
}

std::vector<Entity*> EntityManager::getEntities() {
	return entities;
}

std::vector<Entity*> EntityManager::getVehicleEntities() {
	return vehicleEntities;
}

Entity* EntityManager::getVehicleEntityWithID(int id) {
    for (Entity* ve : vehicleEntities) {
        if (ve->id == id) return ve;
    }
    return nullptr;
}

EntityManager::~EntityManager() = default;

void EntityManager::destroyEntity(int id) {
    // If this entity is contained within the vehicle entities list remove it
    for (uint32_t i = 0; i < vehicleEntities.size(); ++i) {
        if (vehicleEntities[i]->id == id) {
            vehicleEntities.erase(vehicleEntities.begin() + i);
            break;
        }
    }
    // Remove from the general entities list
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

void EntityManager::destroyAllEntities() {
    vehicleEntities.clear();
    for (uint32_t i = 0; i < entities.size(); ++i) {
        Entity* e = entities[i];
        ComponentManager::getInstance()->cleanupComponents(e);
        delete e;
    }
    entities.clear();
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
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("chassis-lvl1"), shapes[4], glm::vec3(1.f), glm::vec3(0), CHASSIS);
	ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("gunHolder-lvl1"), shapes[4], glm::vec3(1.f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("chassisWires-lvl1"), shapes[4], glm::vec3(1.f));

    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("ram1"), shapes[5], glm::vec3(1.f), glm::vec3(0.f, 1.25f, -3.5f), RAM);
	ShapeRendererComponent* tempWeapon = ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("gun1"), shapes[5], glm::vec3(1.f), glm::vec3(0, 3.45f, -1.8f), GUN);

    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[0], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.4, 0, -1.5f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[1], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.5f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[2], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(-0.4, 0, -1.2f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity, ModelManager::getModel("wheels"), shapes[3], glm::vec3(0.4f, 0.8f, 0.8f), glm::vec3(0.4, 0, -1.2f));
    ComponentManager::getInstance()->addPhysicsComponent(entity, rigid1);
    ComponentManager::getInstance()->addDriveComponent(entity, &myVehicleData->myInput, myVehicleData->myVehicle);
    ComponentManager::getInstance()->addHealthComponent(entity, 200.f);
    UpgradeComponent* uc = ComponentManager::getInstance()->addUpgradeComponent(entity);
    WeaponComponent* wc = ComponentManager::getInstance()->addWeaponComponent(entity, tempWeapon);

    // Set the gun to the default values
    wc->updateGunValues(uc->getCurrentGunROF(), uc->getCurrentGunDamage(), uc->getCurrentProjSpeed());

	vehicleEntities.push_back(entity);
    return entity;
}

Entity* EntityManager::createProjectile(int ownerid, glm::vec3 origin, glm::quat orientation, float speed, float damage) {
    Entity* projectile = EntityManager::getInstance()->createEntity(origin, orientation, glm::vec3(1));
	Model* projectileModel;
	glm::vec3 scale(1,1,12);
	if (damage > 7.f) {
		projectileModel = ModelManager::getModel("projectileBig");
		scale = glm::vec3(1.5,1.5,16);
	} else {
        projectileModel = ModelManager::getModel("projectile");
	}
    RendererComponent* temp = ComponentManager::getInstance()->addRendererComponent(projectile, projectileModel, glm::vec3(0), glm::quat(), scale);
    ComponentManager::getInstance()->addProjectileComponent(projectile, ownerid, speed, damage);

    AudioEvent * fireEvent = new AudioEvent(origin, AudioPaths::getInstance()->rifleShot);
    //std::cout << "attempting to create audio event for " + fireEvent->soundfile + " at position (" + std::to_string((fireEvent->position.x)) + ", " + std::to_string((fireEvent->position.y)) + ", " + std::to_string((fireEvent->position.z)) + ")." << std::endl;
    audioEvents->notifyAudioObservers(*fireEvent);

    return projectile;
}

Entity* EntityManager::createPlayerVehicleEntity(glm::vec3 startPos) {
	Entity* playerCar = createBasicVehicleEntity(startPos);

	ComponentManager::getInstance()->addControllableComponent(playerCar, true);
    // DONT ADD ENTITY TO LIST, done in previous call
	return playerCar;
}

Entity* EntityManager::createAIVehicleEntity(glm::vec3 startPos) {
    Entity* aiCar = createBasicVehicleEntity(startPos);

    // We now have a regular car, lets AI-ify it.
    ComponentManager::getInstance()->addAIComponent(aiCar);
	ComponentManager::getInstance()->addControllableComponent(aiCar, false);
    // DONT ADD ENTITY TO LIST, done in previous call
    return aiCar;
}

Entity* EntityManager::createGroundPlane() {
	Entity* entity = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.0f));
	RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("plane2"), glm::vec3(0, 0, 0), glm::quat(), glm::vec3(1, 10, 1));
    rc->isStatic = true;
    physx::PxRigidActor* plane = PhysicsEngine::getInstance()->createPhysicsPlane();
    plane->userData = entity;
	ComponentManager::getInstance()->addPhysicsComponent(entity, plane);
	return entity;
}

Entity* EntityManager::createBox(glm::vec3 startPos, glm::vec3 scale) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);
    RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), glm::vec3(1));
    rc->isStatic = true;
    physx::PxRigidActor* box = PhysicsEngine::getInstance()->createPhysicsBox(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(scale));
    box->userData = entity;
    ComponentManager::getInstance()->addPhysicsComponent(entity, box);
	return entity;
}

Entity* EntityManager::createBoulder(glm::vec3 startPos, glm::vec3 scale) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);

	// randomly choose obstacle type and rotation
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());     // provide seed
	std::uniform_int_distribution<int> uid{ 1, 3};
	std::string s = std::to_string(uid(dre));
	std::uniform_real_distribution<float> urd{0.f, 3.14f};
	float y = urd(dre);

    RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("obstacle"+s+"-2"), glm::vec3(0), glm::quat(glm::vec3(0, y, 0)), glm::vec3(1));
	rc->isStatic = true;
	// Render the physics hitbox for the boulder
	//ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), vec3(1.5f, 2.f, 1.5f));

	physx::PxRigidActor* box = PhysicsEngine::getInstance()->createPhysicsBox(PhysicsEngine::toPxVec3(startPos), physx::PxVec3(scale.x, scale.y*2.f, scale.z), physx::PxQuat(y, physx::PxVec3(0, 1, 0)));
	box->userData = entity;
	ComponentManager::getInstance()->addPhysicsComponent(entity, box);
	return entity;
}

Entity* EntityManager::createSmallRock(glm::vec3 startPos, glm::vec3 scale) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);

	// randomly choose obstacle type and rotation
	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());     // provide seed
	std::uniform_int_distribution<int> uid{ 1, 3 };
	std::string s = std::to_string(uid(dre));
	std::uniform_real_distribution<float> urd{ 0.f, 3.14f };
	float y = urd(dre);

	RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("smallRock"+s), glm::vec3(0), glm::quat(glm::vec3(0, y, 0)), glm::vec3(1));
	rc->isStatic = true;

	return entity;
}

Entity* EntityManager::createWallBoulder(glm::vec3 startPos, glm::vec3 scale) {
	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), scale);

	std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> uid{ 1, 7 };
	std::string s = std::to_string(uid(dre));
	std::uniform_real_distribution<float> urd{ 0.f, 3.14f };
	float y = urd(dre);

    RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("wall" + s), glm::vec3(0), glm::quat(glm::vec3(0, y, 0)), glm::vec3(1));
	rc->isStatic = true;
	// Render the physics hitbox for the wall
	//ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), vec3(4.5f, 12.f, 4.5f));

	physx::PxRigidActor* box = PhysicsEngine::getInstance()->createPhysicsBox(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(scale*4.5f));
	box->userData = entity;
	ComponentManager::getInstance()->addPhysicsComponent(entity, box);
	return entity;
}

Entity* EntityManager::createCrystal(glm::vec3 startPos, float resourceAmount) {
    if (resourceAmount <= 0.5f || resourceAmount > 2.5f) { // if the value is not within 0.5 and 2.5, generate it.
        resourceAmount = float(rand() % 2000 + 500) / 1000.0f;
    }

    bool isHealthCrystal = rand() % 100 < 15; // 15% of crystals are health crystals 
    if (isHealthCrystal) {
        resourceAmount = 1.25f;
    }


    glm::vec3 physicsScale = glm::vec3(1.0f, 4.0f, 1.0f) * resourceAmount;
    glm::vec3 modelScale = glm::vec3(1.0f) * resourceAmount;
    Model* crystalModel = nullptr;
    float heightOffset = 0;

    if (resourceAmount <= 1.5f) {
        if (isHealthCrystal) {
            crystalModel = ModelManager::getModel("healthCrystal");
        } else {
            crystalModel = ModelManager::getModel("smallCrystal1");
        }
        heightOffset = (physicsScale.y - (modelScale.y + resourceAmount * 0.6f) + 1.5f) / 2.0f;
    } else {
        modelScale *= 0.5f;
        physicsScale *= 1.0f;
        crystalModel = ModelManager::getModel("bigCrystal1");
        heightOffset = (physicsScale.y - (modelScale.y + resourceAmount * 0.5f) + 2.5f) / 2.0f;
    }

    
    startPos.y = resourceAmount * 2.0f;
    Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), glm::vec3(1));
    RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, crystalModel, glm::vec3(0,-heightOffset,0), glm::quat(glm::vec3(0, float(rand() % 314) / 100.0f,0)), modelScale);
    rc->isStatic = true;

	float multi = (resourceAmount * resourceAmount + 0.1f);
    glm::vec3 color(30 * multi, 50 * multi, 150 * multi);
    if (isHealthCrystal) {
        entity->myTags.emplace_back("HealthCrystal");
        color = glm::vec3(200 * multi, 40 * multi, 30 * multi);
    }
    ComponentManager::getInstance()->addStaticLightComponent(entity, entity->getPosition() + glm::vec3(0,1 * resourceAmount,0), color);
	ComponentManager::getInstance()->addHealthComponent(entity, resourceAmount * 100.f, true);

    // Render the physics hitbox for the crystal
    //ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("cube"), glm::vec3(0), glm::quat(), physicsScale);

    physx::PxRigidActor* box = PhysicsEngine::getInstance()->createCrystalBoxCollider(PhysicsEngine::toPxVec3(startPos), PhysicsEngine::toPxVec3(physicsScale));
    box->userData = entity;
    ComponentManager::getInstance()->addPhysicsComponent(entity, box);
    return entity;
}

Entity* EntityManager::createCrystalProp(glm::vec3 startPos) {

	Entity* entity = EntityManager::getInstance()->createEntity(startPos, glm::quat(), glm::vec3(1));
	RendererComponent* rc = ComponentManager::getInstance()->addRendererComponent(entity, ModelManager::getModel("crystalProp"), glm::vec3(0), glm::quat(glm::vec3(0)), glm::vec3(1));
	rc->isStatic = true;
	float multi = 0.1f;
	glm::vec3 color(30*multi, 75*multi, 150*multi);

	ComponentManager::getInstance()->addStaticLightComponent(entity, entity->getPosition() + glm::vec3(0, 2, 0), color);

	return entity;
}

Entity* EntityManager::createFloatingText (Entity* relativeEnt, glm::vec3 color, float scale, std::string* text) {
    Entity* entity = EntityManager::getInstance()->createEntity(relativeEnt->getPosition(), glm::quat(), glm::vec3(1));
    FloatingTextComponent* ftc = ComponentManager::getInstance()->addFloatingTextComponent(entity, relativeEnt, color, scale, text);
    return entity;
}

const glm::vec3 chassisScales[3] = {glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(2.5f, 2.5f, 2.5f), glm::vec3(3.5f, 3.5f, 3.5f)};

void EntityManager::createBrokenVehicle(int chassisLevel, glm::vec3 pos, glm::quat rot, glm::vec3 velocity) {

	glm::vec3 scale = chassisScales[chassisLevel - 1];
    glm::mat4 globalMat;
    globalMat = glm::translate(globalMat, pos) * glm::toMat4(rot) * glm::scale(globalMat, scale);

    // Add the body
    Entity* bodyEnt = createEntity(pos, rot, scale);
    glm::quat bodyRot(glm::toMat4(rot) * glm::toMat4(glm::quat(glm::vec3(0.6f,0,0))));
    physx::PxRigidActor* bodyActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + glm::vec3(0,0.5f,0)), 
        PhysicsEngine::toPxVec3(glm::vec3(1.3f,0.5f,2.5f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(bodyRot.x, bodyRot.y, bodyRot.z, bodyRot.w));
    ComponentManager::getInstance()->addPhysicsComponent(bodyEnt, bodyActor);
    ComponentManager::getInstance()->addShapeRendererComponent(bodyEnt, ModelManager::getModel("blownUpPieces-Chassis2"), PhysicsEngine::getFirstShapeFromActor(bodyActor), glm::vec3(0.7f));

    // Add the thing that holds the gun mount
    Entity* holderEnt = createEntity(pos, rot, scale);
    glm::vec3 holderOffset = glm::vec3(globalMat * glm::vec4(0, 1.6f, -0.4f, 0.0));
    glm::quat holderRot(glm::toMat4(rot) * glm::toMat4(glm::quat(glm::vec3(0.65f,0,0))));
    physx::PxRigidActor* holderActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + holderOffset), 
        PhysicsEngine::toPxVec3(glm::vec3(0.3f, 0.3f, 0.8f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(holderRot.x, holderRot.y, holderRot.z, holderRot.w));
    ComponentManager::getInstance()->addPhysicsComponent(holderEnt, holderActor);
    ComponentManager::getInstance()->addShapeRendererComponent(holderEnt, ModelManager::getModel("blownUpPieces-Chassis1"), PhysicsEngine::getFirstShapeFromActor(holderActor), glm::vec3(0.7f));

    // Add the two rails on the top of the body
    glm::vec3 railOffsets[2] = {glm::vec3(-0.6f,0.85f,0.7f), glm::vec3(0.6f,0.85f,0.7f)};

    for (int i = 0; i < 2; i++) {
        Entity* railEnt = createEntity(pos, rot, scale);
        glm::vec3 railOffset = glm::vec3(globalMat * glm::vec4(railOffsets[i],0.0));
        glm::quat railRot(glm::toMat4(rot) * glm::toMat4(glm::quat(glm::vec3(-0.95f,0,0))));
        physx::PxRigidActor* railActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + railOffset), 
            PhysicsEngine::toPxVec3(glm::vec3(0.2f, 1.5f, 0.2f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(railRot.x, railRot.y, railRot.z, railRot.w));
        ComponentManager::getInstance()->addPhysicsComponent(railEnt, railActor);
        ComponentManager::getInstance()->addShapeRendererComponent(railEnt, ModelManager::getModel("blownUpPieces-Chassis4"), PhysicsEngine::getFirstShapeFromActor(railActor), glm::vec3(0.65f));
    }

    // Add the wheels and the legs
    glm::vec3 legOffsets[4] = {glm::vec3(0.8f,-0.2f,0.5f), glm::vec3(0.8f,-0.1f,-1.f), glm::vec3(-0.8f,-0.2f,0.5f), glm::vec3(-0.8f,-0.1f,-1.f)};
    glm::vec3 legRotOffset[4] = {glm::vec3(-0.5f, 0, 0), glm::vec3(0.1f, 0, 0), glm::vec3(-0.5f, 0, 0), glm::vec3(0.1f, 0, 0)};
    glm::vec3 wheelOffsets[4] = {glm::vec3(1.f,-1.f,1.f), glm::vec3(1.f,-1.f,-1.f), glm::vec3(-1.f,-1.f,1.f), glm::vec3(-1.f,-1.f,-1.f)};

    // Spawn the vehicle legs and wheels
    for (int i = 0; i < 4; i++) {
        Entity* legEnt = createEntity(pos, rot, scale);
        glm::quat legRot = glm::quat(glm::toMat4(rot) * glm::toMat4(glm::quat(legRotOffset[i])));
        glm::vec3 legOffset = glm::vec3(globalMat * glm::vec4(legOffsets[i],0.0));
        physx::PxRigidActor* legActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + legOffset),
            PhysicsEngine::toPxVec3(glm::vec3(0.2f, 1.f, 0.2f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(legRot.x, legRot.y, legRot.z, legRot.w));
        ComponentManager::getInstance()->addPhysicsComponent(legEnt, legActor);
        ComponentManager::getInstance()->addShapeRendererComponent(legEnt, ModelManager::getModel("blownUpPieces-Chassis3"), PhysicsEngine::getFirstShapeFromActor(legActor), glm::vec3(0.8f));


        Entity* wheelEnt = createEntity(pos, rot, scale);
        glm::vec3 wheelOffset = glm::vec3(globalMat * glm::vec4(wheelOffsets[i],0.0));
        physx::PxRigidActor* wheelActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wheelOffset), 
            PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.55f, 0.55f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
        ComponentManager::getInstance()->addPhysicsComponent(wheelEnt, wheelActor);
        ComponentManager::getInstance()->addShapeRendererComponent(wheelEnt, ModelManager::getModel("wheels"), PhysicsEngine::getFirstShapeFromActor(wheelActor), glm::vec3(0.4f, 0.55f, 0.55f));
    }

    // Here we render the wires, the number is dependent on the level of the chassis being destroyed
    for (int i = 0; i < chassisLevel * 4; i++) {
        float x = (float(rand() % 50) / 50.f) - 0.5f;
        float z = (float(rand() % 100) / 50.f) - 1.f;
        float y = z * -1.f + 0.5f;
        glm::vec3 wirePos(x,y,z);
        if (i < 4) { // lvl1
            // spawn 1 and 3, even distribution
            if (rand() % 2 == 0) { // 50 - 50 chance
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.4f, 0.4f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires1"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            } else {
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.5f, 0.5f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires3"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            }
        } else if (i < 8) { // lvl2
            // spawn 3 and 4, mostly 4
            if (rand() % 3 < 1) { // 66% chance of 4
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.6f, 0.6f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires4"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            } else {
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.5f, 0.5f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires3"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            }
        } else if (i < 12) { // lvl3
            // spawn 2 and 4, mostly 2
            if (rand() % 3 < 1) { // 66% chance of 2
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.3f, 0.7f, 0.7f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires2"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            } else {
                Entity* wireEnt = createEntity(pos, rot, scale);
                glm::vec3 wireOffset = glm::vec3(globalMat * glm::vec4(wirePos,0.0));
                physx::PxRigidActor* wireActor = PhysicsEngine::getInstance()->createDynamicPhysicsBox(10.f, PhysicsEngine::toPxVec3(pos + wireOffset), 
                    PhysicsEngine::toPxVec3(glm::vec3(0.2f, 0.5f, 0.5f) * scale), PhysicsEngine::toPxVec3(velocity), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
                ComponentManager::getInstance()->addPhysicsComponent(wireEnt, wireActor);
                ComponentManager::getInstance()->addShapeRendererComponent(wireEnt, ModelManager::getModel("blownUpPieces-ChassisWires3"), PhysicsEngine::getFirstShapeFromActor(wireActor), glm::vec3(0.8f));
            }
        }
    }

    // If the vehicle had armour then add some scales. Unlike everything else, the size of the scales should not change, just the quantity
    // This is OPTIONAL but would look pretty cool.
}

// ***************************************************** SECTION FOR UPGRADING/MODIFYING VEHICLE PARTS *******************************************************************
void EntityManager::updateChassis (Entity* toUpdate, float newScale, int level) {
    // First thing we need to do is grab the renderer for the chassis and update it.
    // Then we need to tell physics to scale the vehicle.
    // Then we need to scale all of the renderer components scale and local positions accordingly.

    // Get the chassis, remove it from the render engine, update the model, add it back to the render engine
    ShapeRendererComponent* chassis = static_cast<ShapeRendererComponent*>(ComponentManager::getInstance()->getRenderComponentWithTagFromEntity(toUpdate, CHASSIS));
    Core::renderEngine->world->removeInstance(*chassis->myModel, chassis->id);
    switch(level) {
        case 1:
            chassis->myModel = ModelManager::getModel("chassis-lvl1");
        break;
        case 2:
            chassis->myModel = ModelManager::getModel("chassis-lvl2");
        break;
        case 3:
            chassis->myModel = ModelManager::getModel("chassis-lvl3");
        break;
    }
    Core::renderEngine->world->addInstance(*chassis->myModel, chassis->id, chassis->getMatrix());

    // Now iterate through every ShapeRendererComponent and modify the relevant scalable values. This includes the chassis.
    for (auto & myComponent : toUpdate->myComponents) {
        if (myComponent->getComponentType() == RENDERER) {
            ShapeRendererComponent* src = static_cast<ShapeRendererComponent*>(myComponent);
            src->scale *= newScale;
            src->localPos *= newScale;
        }
    }

    // Now change physics. Move up 2 spaces to account for enlarging. This should be more precisely tuned later.
    physx::PxRigidBody* rigidBody = (static_cast<PhysicsComponent*>(toUpdate->getComponent(PHYSICS)))->getRigidBody();
    const physx::PxTransform toMovePos(rigidBody->getGlobalPose().p + physx::PxVec3(0, 2, 0),rigidBody->getGlobalPose().q);
    rigidBody->setGlobalPose(toMovePos);
    physx::PxRigidDynamic* rigidDyn = static_cast<physx::PxRigidDynamic*>(rigidBody);
    physx::PxVehicleDrive4W* vehicle = (static_cast<DriveComponent*>(toUpdate->getComponent(DRIVE)))->getVehicle();
    PhysicsEngine::getInstance()->modifyVehicleScale(newScale, rigidDyn, vehicle);
}

void EntityManager::updateGun (Entity* toUpdate, int gunLevel) {
    ShapeRendererComponent* gun = static_cast<ShapeRendererComponent*>(ComponentManager::getInstance()->getRenderComponentWithTagFromEntity(toUpdate, GUN));
    Core::renderEngine->world->removeInstance(*gun->myModel, gun->id);
    switch(gunLevel) {
    case 1:
        gun->myModel = ModelManager::getModel("gun1");
        break;
    case 2:
        gun->myModel = ModelManager::getModel("gun2");
        break;
    case 3:
        gun->myModel = ModelManager::getModel("gun3");
        break;
    case 4:
        gun->myModel = ModelManager::getModel("gun4");
        break;
    case 5:
        gun->myModel = ModelManager::getModel("gun5");
        break;
    }
    Core::renderEngine->world->addInstance(*gun->myModel, gun->id, gun->getMatrix());
}

void EntityManager::updateRam (Entity* toUpdate, int ramLevel) {
    ShapeRendererComponent* ram = static_cast<ShapeRendererComponent*>(ComponentManager::getInstance()->getRenderComponentWithTagFromEntity(toUpdate, RAM));
    Core::renderEngine->world->removeInstance(*ram->myModel, ram->id);
    switch(ramLevel) {
    case 1:
        ram->myModel = ModelManager::getModel("ram1");
        break;
    case 2:
        ram->myModel = ModelManager::getModel("ram2");
        break;
    case 3:
        ram->myModel = ModelManager::getModel("ram3");
        break;
    case 4:
        ram->myModel = ModelManager::getModel("ram4");
        break;
    case 5:
        ram->myModel = ModelManager::getModel("ram5");
        break;
    }
    Core::renderEngine->world->addInstance(*ram->myModel, ram->id, ram->getMatrix());
}

void EntityManager::updateArmor (Entity* toUpdate, int chassisLevel, int armorLevel) {

    // Grab the chassis to check for scale and PxShape
    ShapeRendererComponent* chassisSR = static_cast<ShapeRendererComponent*>(ComponentManager::getInstance()->getRenderComponentWithTagFromEntity(toUpdate, CHASSIS));

    // Create the a new ShapeRendererComponent with the approppriate scale and pxShape.
    ShapeRendererComponent* addedArmor = nullptr;
    switch(armorLevel) {
    case 2:
        addedArmor = ComponentManager::getInstance()->addShapeRendererComponent(toUpdate, ModelManager::getModel("armour-lvl1"), chassisSR->getMyShape(), chassisSR->scale, glm::vec3(0));
        break;
    case 3:
        addedArmor = ComponentManager::getInstance()->addShapeRendererComponent(toUpdate, ModelManager::getModel("armour-lvl2"), chassisSR->getMyShape(), chassisSR->scale, glm::vec3(0));
        break;
    case 4:
        addedArmor = ComponentManager::getInstance()->addShapeRendererComponent(toUpdate, ModelManager::getModel("armour-lvl3"), chassisSR->getMyShape(), chassisSR->scale, glm::vec3(0));
        break;
    case 5:
        addedArmor = ComponentManager::getInstance()->addShapeRendererComponent(toUpdate, ModelManager::getModel("armour-lvl4"), chassisSR->getMyShape(), chassisSR->scale, glm::vec3(0));
        addedArmor = ComponentManager::getInstance()->addShapeRendererComponent(toUpdate, ModelManager::getModel("armour-lvl5"), chassisSR->getMyShape(), chassisSR->scale, glm::vec3(0));
        break;
    }
}
