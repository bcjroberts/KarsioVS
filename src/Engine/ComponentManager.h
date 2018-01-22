#pragma once

#include "vector"
#include "../game/components/RendererComponent.h"
#include  "RenderEngine/RenderEngine.h"
#include <PxRigidDynamic.h>
#include "../game/components/PhysicsComponent.h"

class ComponentManager
{
private:
    std::vector<RendererComponent*> rendererComponents;
    std::vector<PhysicsComponent*> physicsComponents;
    static ComponentManager* globalInstance;
public:
    ComponentManager();
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader);
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    PhysicsComponent* addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor);
    void performRendering(RenderEngine* re);
    void initializeRendering(RenderEngine* re);
    void performPhysicsLogic();
    static ComponentManager* getInstance();
    ~ComponentManager();
};

