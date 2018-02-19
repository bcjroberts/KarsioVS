#include "ComponentManager.h"
#include "Entity.h"
#include "../Game/Components/HealthComponent.h"

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

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, Model* mesh, physx::PxShape* newShape, glm::vec3 newScale)
{
    ShapeRendererComponent* src = new ShapeRendererComponent(mesh, newShape);
    rendererComponents.push_back(src);
    addTo->addComponent(src);

    // SPECIAL: set the scale
    src->scale = newScale;

    src->owner = addTo;
    return src;
}

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, Model* model, physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos)
{
    ShapeRendererComponent* src = addShapeRendererComponent(addTo, model, newShape, newScale);
    src->localPos = newLocalPos;
    return src;
}

PhysicsComponent* ComponentManager::addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor) {
    PhysicsComponent* pc = new PhysicsComponent(nactor);
    physicsComponents.push_back(pc);
    addTo->addComponent(pc);
    pc->owner = addTo;
    return pc;
}

DriveComponent* ComponentManager::addDriveComponent(Entity* addTo, physx::PxVehicleDrive4WRawInputData* inputData) {
    DriveComponent* dc = new DriveComponent(inputData);
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

        // ********************** TEMPORARY ************************
        // Hide the destroyed object below the map
        renderEngine->updateInstance(*rc->myModel, rc->id, hiddenMat4);

        // This is the spot where I would remove the component from the renderer rc->myModel;
        for (int i = 0; i < rendererComponents.size(); ++i)
        {
            if (rendererComponents[i]->id == rc->id) {
                rendererComponents.erase(rendererComponents.begin() + i);
                break;
            }
        }
        entity->removeComponent(toRemove);
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
    }
}
