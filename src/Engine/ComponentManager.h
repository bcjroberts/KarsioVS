#pragma once

#include "vector"
#include "../game/components/RendererComponent.h"
#include  "RenderEngine/RenderEngine.h"

class ComponentManager
{
private:
    std::vector<RendererComponent*> rendererComponents;
    static ComponentManager* globalInstance;
public:
    ComponentManager();
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader);
    RendererComponent* addRendererComponent(Entity* addTo, MeshData* mesh, ShaderData* shader, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    void performRendering(RenderEngine* re);
    void initializeRendering(RenderEngine* re);
    static ComponentManager* getInstance();
    ~ComponentManager();
};

