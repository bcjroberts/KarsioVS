#pragma once

#include "vector"
#include "../game/components/RendererComponent.h"
#include "../game/components/ShapeRendererComponent.h"
#include  "RenderEngine/RenderEngine.h"
#include <PxRigidDynamic.h>
#include "../game/components/PhysicsComponent.h"
#include "../Game/Components/DriveComponent.h"
#include "../Game/Components/HealthComponent.h"
#include "../Game/Components/AIComponent.h"
#include "../Game/Components/ControllableComponent.h"

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
    RendererComponent* addRendererComponent(Entity* addTo, Model* model);
    RendererComponent* addRendererComponent(Entity* addTo, Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    ShapeRendererComponent* addShapeRendererComponent(Entity* addTo, Model* mesh, physx::PxShape* newShape, glm::vec3 newScale);
    ShapeRendererComponent* addShapeRendererComponent(Entity* addTo, Model* mesh,  physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos);
    PhysicsComponent* addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor);
    DriveComponent* addDriveComponent(Entity* addTo, physx::PxVehicleDrive4WRawInputData* inputData);
    HealthComponent* addHealthComponent(Entity* addTo, float health, bool healthIsThreshold = false);
    AIComponent* addAIComponent(Entity* addTo);
	ControllableComponent* addControllableComponent(Entity* addTo, bool player);
    void performRendering();
    void initializeRendering(RenderEngine* re);
    void performPhysicsLogic();
    void cleanupComponents(Entity* entity);
    static ComponentManager* getInstance();
    ~ComponentManager();
};

