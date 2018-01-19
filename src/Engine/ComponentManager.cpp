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
    rc->position = position;
    rc->rotation = rotation;
    rc->scale = scale;
    return rc;
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
