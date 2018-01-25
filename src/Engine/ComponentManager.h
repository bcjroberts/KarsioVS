#pragma once

#include "vector"
#include "../game/components/RendererComponent.h"
#include "../game/components/ShapeRendererComponent.h"
#include  "RenderEngine/RenderEngine.h"
#include <PxRigidDynamic.h>
#include "../game/components/PhysicsComponent.h"
#include "../Game/Components/DriveComponent.h"

class ComponentManager
{
private:
    std::vector<RendererComponent*> rendererComponents;
    std::vector<PhysicsComponent*> physicsComponents;
    std::vector<DriveComponent*> driveComponents;
    static ComponentManager* globalInstance;
public:
    ComponentManager();
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader);
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    ShapeRendererComponent* addShapeRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, physx::PxShape* newShape, physx::PxRigidActor* newActor, glm::vec3 newScale);
    PhysicsComponent* addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor);
    void performRendering(RenderEngine* re);
    void initializeRendering(RenderEngine* re);
    void performPhysicsLogic();
    static ComponentManager* getInstance();
    ~ComponentManager();
};

