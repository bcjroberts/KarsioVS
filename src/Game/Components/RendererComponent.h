#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "../../Engine/Importer/Data//ModelData.h"
#include "../../Engine/Importer/Data/ShaderData.h"

class RendererComponent : public Component {
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
    ModelData* myModel;
    virtual mat4 getMatrix();
	RendererComponent(ModelData* newModel);
	~RendererComponent();
};

