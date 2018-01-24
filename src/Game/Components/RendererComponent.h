#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "../../Art/MeshData.h"
#include "../../Art/ShaderData.h"

class RendererComponent : public Component
{
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
    MeshData* myMesh;
	ShaderData* myShader;
    virtual mat4 getMatrix();
	RendererComponent(MeshData* newMesh, ShaderData* newShader);
	~RendererComponent();
};

