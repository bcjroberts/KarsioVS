#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"
#include "../../Engine/Importer/Data//Model.h"
#include "../../Engine/Importer/Data/ShaderData.h"

// NA is default, the others refer to parts that could be changed at runtime
enum RendererTag {NA, GUN, GUNHOLDER, ARMOR, RAM, CHASSIS};

class RendererComponent : public Component {
public:
    RendererTag tag = NA;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
    Model* myModel;
    bool isStatic;
    virtual glm::mat4 getMatrix();
	RendererComponent(Model* newModel, RendererTag newTag = NA);
	virtual ~RendererComponent();
};

