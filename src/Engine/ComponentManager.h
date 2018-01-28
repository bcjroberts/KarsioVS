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
    static ComponentManager* globalInstance;
public:
    std::vector<RendererComponent*> rendererComponents;
    std::vector<PhysicsComponent*> physicsComponents;
    std::vector<DriveComponent*> driveComponents;

    // Components moved here temporarily

    ComponentManager();
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader);
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    ShapeRendererComponent* addShapeRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, physx::PxShape* newShape, glm::vec3 newScale);
    PhysicsComponent* addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor);
    DriveComponent* addDriveComponent(Entity* addTo, physx::PxVehicleDrive4WRawInputData* inputData);
    void performRendering(RenderEngine* re);
    void initializeRendering(RenderEngine* re);
    void performPhysicsLogic();
    static ComponentManager* getInstance();
    ~ComponentManager();
};

