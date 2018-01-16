#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "../../Art/MeshData.h"

class RendererComponent : public Component
{
private:
	int eventBitmask;
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	MeshData* myMesh;
	RendererComponent();
	~RendererComponent();
	int getEventBitmask();
};

