#include "ComponentManager.h"
#include "Entity.h"

// Initialize the Component Manager global pointer.
ComponentManager *ComponentManager::globalInstance = nullptr;

ComponentManager::ComponentManager() = default;


ComponentManager::~ComponentManager() = default;

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, Model* model) {
    return addRendererComponent(addTo, model, glm::vec3(), glm::quat(), glm::vec3());
}

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, Model* model,  glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    RendererComponent* rc = new RendererComponent(model);
    rendererComponents.push_back(rc);
    addTo->addComponent(rc);
    rc->position = position;
    rc->rotation = rotation;
    rc->scale = scale;
    rc->owner = addTo;
    return rc;
}

RenderEngine* renderEngine = nullptr;

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, Model* model, physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos, RendererTag tag)
{
    ShapeRendererComponent* src = new ShapeRendererComponent(model, newShape, tag);
    rendererComponents.push_back(src);
    addTo->addComponent(src);

    // SPECIAL: set the scale and local position if given
    src->scale = newScale;
    src->localPos = newLocalPos;

    src->owner = addTo;
    return src;
}

PhysicsComponent* ComponentManager::addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor) {
    PhysicsComponent* pc = new PhysicsComponent(nactor);
    physicsComponents.push_back(pc);
    addTo->addComponent(pc);
    pc->owner = addTo;
    return pc;
}

DriveComponent* ComponentManager::addDriveComponent(Entity* addTo, physx::PxVehicleDrive4WRawInputData* inputData, physx::PxVehicleDrive4W* vehicle) {
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
        renderEngine->updateInstance(*rc->myModel, rc->id, rc->getMatrix());
    }
}

void ComponentManager::initializeRendering(RenderEngine* re) {
    renderEngine = re;
    for (auto rc : rendererComponents) {
        re->addInstance(*rc->myModel, rc->id, rc->getMatrix());
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
        renderEngine->removeInstance(*rc->myModel, rc->id);

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
        entity->removeComponent(toRemove->id);
        delete toRemove;
    }

    // Now we can just delete the other components without any special treatment
}
