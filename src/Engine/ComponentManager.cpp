#include "ComponentManager.h"

// Initialize the Component Manager global pointer.
ComponentManager *ComponentManager::globalInstance = nullptr;

ComponentManager::ComponentManager() = default;


ComponentManager::~ComponentManager() = default;

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader) {
    return addRendererComponent(addTo, mesh, shader, glm::vec3(), glm::quat(), glm::vec3());
}

RendererComponent* ComponentManager::addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    RendererComponent* rc = new RendererComponent(mesh, shader);
    rendererComponents.push_back(rc);
    addTo->addComponent(rc);
    rc->position = position;
    rc->rotation = rotation;
    rc->scale = scale;
    rc->owner = addTo;
    return rc;
}

PhysicsComponent* ComponentManager::addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor) {
    PhysicsComponent* pc = new PhysicsComponent(nactor);
    physicsComponents.push_back(pc);
    addTo->addComponent(pc);
    pc->owner = addTo;
    return pc;
}

ComponentManager* ComponentManager::getInstance() {
    if (!globalInstance) {
        globalInstance = new ComponentManager;
    }
    return globalInstance;
}

void ComponentManager::performRendering(RenderEngine* re) {
    for (auto rc : rendererComponents) {
        re->updateInstance(*rc->myMesh, rc->id, rc->getMatrix());
    }
}

void ComponentManager::initializeRendering(RenderEngine* re) {
    for (auto rc : rendererComponents) {
        re->addInstance(*rc->myMesh, rc->id, rc->getMatrix(),*rc->myShader);
    }
}

void ComponentManager::performPhysicsLogic() {
    for (auto pc : physicsComponents) {
        physx::PxTransform gp = pc->myActor->getGlobalPose();
        pc->owner->updatePosition(glm::vec3(gp.p.x,gp.p.y,gp.p.z));
        pc->owner->updateRotation(glm::quat(gp.q.w, gp.q.x, gp.q.y, gp.q.z));
    }
}
