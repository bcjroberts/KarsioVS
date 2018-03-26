#include "ComponentManager.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Core.h"

// Initialize the Component Manager global pointer.
ComponentManager *ComponentManager::globalInstance = nullptr;

ComponentManager::ComponentManager() = default;


ComponentManager::~ComponentManager() = default;

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, Model* model) {
    return addRendererComponent(addTo, model, glm::vec3(), glm::quat(), glm::vec3(1));
}

void ComponentManager::registerRenderComponent(RendererComponent* rc) {
    rendererComponents.push_back(rc);
    Core::renderEngine->world->addInstance(*rc->myModel, rc->id, rc->getMatrix());
}

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, Model* model,  glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    RendererComponent* rc = new RendererComponent(model);
    addTo->addComponent(rc);
    rc->position = position;
    rc->rotation = rotation;
    rc->scale = scale;
    rc->owner = addTo;

    registerRenderComponent(rc);
    return rc;
}

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, Model* model, physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos, RendererTag tag)
{
    ShapeRendererComponent* src = new ShapeRendererComponent(model, newShape, tag);
    addTo->addComponent(src);

    // SPECIAL: set the scale and local position if given
    src->scale = newScale;
    src->localPos = newLocalPos;

    src->owner = addTo;

    registerRenderComponent(src);
    return src;
}

PhysicsComponent* ComponentManager::addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor) {
    PhysicsComponent* pc = new PhysicsComponent(nactor);
    physicsComponents.push_back(pc);
    addTo->addComponent(pc);
    pc->owner = addTo;
    return pc;
}

DriveComponent* ComponentManager::addDriveComponent(Entity* addTo, vehicleInput* inputData, physx::PxVehicleDrive4W* vehicle) {
    DriveComponent* dc = new DriveComponent(inputData, vehicle);
    driveComponents.push_back(dc);
    addTo->addComponent(dc);
    dc->owner = addTo;
    return dc;
}

HealthComponent* ComponentManager::addHealthComponent(Entity* addTo, float health, bool healthIsThreshold) {
    HealthComponent* hc = new HealthComponent(health, healthIsThreshold);
    addTo->addComponent(hc);
    hc->owner = addTo;
    return hc;
}

AIComponent* ComponentManager::addAIComponent(Entity* addTo) {
    AIComponent* aic = new AIComponent();
    addTo->addComponent(aic);
    aic->owner = addTo;
    return aic;
}

ControllableComponent* ComponentManager::addControllableComponent(Entity* addTo, bool player) {
	ControllableComponent* cc = new ControllableComponent(player);
	addTo->addComponent(cc);
	cc->owner = addTo;
	return cc;
}

UpgradeComponent* ComponentManager::addUpgradeComponent(Entity* addTo) {
    UpgradeComponent* uc = new UpgradeComponent();
    addTo->addComponent(uc);
    uc->owner = addTo;
    return uc;
}

WeaponComponent* ComponentManager::addWeaponComponent(Entity* addTo) {
    WeaponComponent* wc = new WeaponComponent();
    addTo->addComponent(wc);
    wc->owner = addTo;
    return wc;
}

ProjectileComponent* ComponentManager::addProjectileComponent(Entity* addTo, int ownerid, float speed, float damage) {
    ProjectileComponent* pc = new ProjectileComponent(ownerid, speed, damage);
    projectiles.push_back(pc);
    addTo->addComponent(pc);
    pc->owner = addTo;
    return pc;
}

StaticLightComponent* ComponentManager::addStaticLightComponent(Entity* entity, glm::vec3 pos, glm::vec3 color) {
    StaticLightComponent* slc = new StaticLightComponent(pos, color);
    entity->addComponent(slc);
    slc->owner = entity;
    return slc;
}

RendererComponent* ComponentManager::getRenderComponentWithTagFromEntity(Entity* from, RendererTag tag) {
    for (auto & myComponent : from->myComponents)
    {
        if (myComponent->getComponentType() == RENDERER)
        {
            RendererComponent* renderComponent = static_cast<RendererComponent*>(myComponent);
            if (renderComponent->tag == tag) {
                return renderComponent;
            }
        }
    }
    return nullptr;
}

ComponentManager* ComponentManager::getInstance() {
    if (!globalInstance) {
        globalInstance = new ComponentManager;
    }
    return globalInstance;
}

void ComponentManager::performRendering() {
    for (auto rc : rendererComponents) {
        if (!rc->isStatic) { // Only update instances that are not static
            Core::renderEngine->world->updateInstance(*rc->myModel, rc->id, rc->getMatrix());
        }
    }
}

void ComponentManager::performPhysicsLogic() {
    for (auto pc : physicsComponents) {
        physx::PxTransform gp = pc->myActor->getGlobalPose();
        pc->owner->updatePosition(glm::vec3(gp.p.x,gp.p.y,gp.p.z));
        pc->owner->updateRotation(glm::quat(gp.q.w, gp.q.x, gp.q.y, gp.q.z));
    }
}

glm::mat4 hiddenMat4 = glm::translate(glm::mat4(), glm::vec3(0, -25, 0));

// Removes all of the components, and cleans up their traces
void ComponentManager::cleanupComponents(Entity* entity) {

    Component* toRemove = entity->getComponent(RENDERER);
    while (toRemove != nullptr) {
        RendererComponent* rc = static_cast<RendererComponent*>(toRemove);

        // destroy the object
        Core::renderEngine->world->removeInstance(*rc->myModel, rc->id);

        // This is the spot where I would remove the component from the renderer rc->myModel;
        for (int i = 0; i < rendererComponents.size(); ++i)
        {
            if (rendererComponents[i]->id == rc->id) {
                rendererComponents.erase(rendererComponents.begin() + i);
                break;
            }
        }
        entity->removeComponent(toRemove->id);
		delete toRemove;
        toRemove = entity->getComponent(RENDERER);
    }

    toRemove = entity->getComponent(PHYSICS);
    if (toRemove != nullptr) {
        for (int i = 0; i < physicsComponents.size(); ++i) {
            if (physicsComponents[i]->id == toRemove->id) {
                physicsComponents.erase(physicsComponents.begin() + i);
                break;
            }
        }
        // Tell the physics engine to remove the actor from the physics simulation
        PhysicsEngine::getInstance()->removePhysicsActor(static_cast<PhysicsComponent*>(toRemove)->myActor);
        entity->removeComponent(toRemove->id);
        delete toRemove;
    }

    toRemove = entity->getComponent(DRIVE);
    if (toRemove != nullptr) {
        for (int i = 0; i < driveComponents.size(); ++i) {
            if (driveComponents[i]->id == toRemove->id) {
                driveComponents.erase(driveComponents.begin() + i);
                break;
            }
        }
        // Remove the vehicle from the physx list so it is no longer updated
        PhysicsEngine::getInstance()->removeVehicleData(static_cast<DriveComponent*>(toRemove)->getVehicle());
        entity->removeComponent(toRemove->id);
        delete toRemove;
    }

    // Remove the projectile component. Mostly important for replaying.
    toRemove = entity->getComponent(PROJECTILE);
    if (toRemove != nullptr) {
        for (int i = 0; i < projectiles.size(); ++i) {
            if (projectiles[i]->id == toRemove->id) {
                projectiles.erase(projectiles.begin() + i);
                break;
            }
        }
        // Remove the vehicle from the physx list so it is no longer updated
        entity->removeComponent(toRemove->id);
        delete toRemove;
    }

    // Now we can just delete the other components without any special treatment
}

void ComponentManager::performProjectileLogic() {
    std::vector<ProjectileComponent*>::iterator it = projectiles.begin();
    
    // Iterates through all of the projectiles and removes them on the fly.
    while (it != projectiles.end()) {
        ProjectileComponent* temp = (*it);
        if (temp->checkForHit()) {
            it = projectiles.erase(it);
            EntityManager::getInstance()->destroyEntity(temp->owner->id);
        } else {
            temp->move();
            ++it;
        }
    }
}

