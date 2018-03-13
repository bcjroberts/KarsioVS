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
#include "../Game/Components/UpgradeComponent.h"
#include "../Game/Components/WeaponComponent.h"
#include "../Game/Components/ProjectileComponent.h"

class ComponentManager
{
private:
    static ComponentManager* globalInstance;
public:
    std::vector<RendererComponent*> rendererComponents;
    std::vector<PhysicsComponent*> physicsComponents;
    std::vector<DriveComponent*> driveComponents;
    std::vector<ProjectileComponent*> projectiles;

    // Components moved here temporarily

    ComponentManager();
    RendererComponent* addRendererComponent(Entity* addTo, Model* model);
    RendererComponent* addRendererComponent(Entity* addTo, Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    ShapeRendererComponent* addShapeRendererComponent(Entity* addTo, Model* model,  physx::PxShape* newShape, glm::vec3 newScale, glm::vec3 newLocalPos = glm::vec3(0), RendererTag tag = NA);
    PhysicsComponent* addPhysicsComponent(Entity* addTo, physx::PxRigidActor* nactor);
    DriveComponent* addDriveComponent(Entity* addTo, vehicleInput* inputData, physx::PxVehicleDrive4W* vehicle);
    HealthComponent* addHealthComponent(Entity* addTo, float health, bool healthIsThreshold = false);
    AIComponent* addAIComponent(Entity* addTo);
	ControllableComponent* addControllableComponent(Entity* addTo, bool player);
    UpgradeComponent* addUpgradeComponent(Entity* addTo);
    WeaponComponent* addWeaponComponent(Entity* addTo);
    ProjectileComponent* addProjectileComponent(Entity* entity, int ownerid, float speed, float damage);

    RendererComponent* getRenderComponentWithTagFromEntity(Entity* from, RendererTag tag);
    void performRendering();
    void initializeRendering(RenderEngine* re);
    void performPhysicsLogic();
    void cleanupComponents(Entity* entity);
    void performProjectileLogic();
    static ComponentManager* getInstance();
    ~ComponentManager();
};

