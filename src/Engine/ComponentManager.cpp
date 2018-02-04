#include "ComponentManager.h"
#include "Entity.h"

// Initialize the Component Manager global pointer.
ComponentManager *ComponentManager::globalInstance = nullptr;

ComponentManager::ComponentManager() = default;


ComponentManager::~ComponentManager() = default;

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, ModelData* model) {
    return addRendererComponent(addTo, model, glm::vec3(), glm::quat(), glm::vec3());
}

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, ModelData* model,  glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    RendererComponent* rc = new RendererComponent(model);
    rendererComponents.push_back(rc);
    addTo->addComponent(rc);
    rc->position = position;
    rc->rotation = rotation;
    rc->scale = scale;
    rc->owner = addTo;
    return rc;
}

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, ModelData* mesh, physx::PxShape* newShape, glm::vec3 newScale)
{
    ShapeRendererComponent* src = new ShapeRendererComponent(mesh, newShape);
    rendererComponents.push_back(src);
    addTo->addComponent(src);

    // SPECIAL: set the scale
    src->scale = newScale;

    src->owner = addTo;
    return src;
}

ShapeRendererComponent* ComponentManager::addShapeRendererComponent(Entity* addTo, ModelData* model, physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos)
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

ComponentManager* ComponentManager::getInstance() {
    if (!globalInstance) {
        globalInstance = new ComponentManager;
    }
    return globalInstance;
}

void ComponentManager::performRendering(RenderEngine* re) {
    for (auto rc : rendererComponents) {
        re->updateInstance(*rc->myModel, rc->id, rc->getMatrix());
    }
}

void ComponentManager::initializeRendering(RenderEngine* re) {
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
