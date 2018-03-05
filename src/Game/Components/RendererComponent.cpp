#include "RendererComponent.h"
#include <glm/gtx/quaternion.hpp>
#include "../../Engine/Entity.h"

mat4 RendererComponent::getMatrix() {
    mat4 myMatrix;
    myMatrix = glm::translate(myMatrix, position) * glm::toMat4(rotation) * glm::scale(myMatrix, scale);
    return owner->getMatrix() * myMatrix;
}

RendererComponent::RendererComponent(Model* newModel, RendererTag newTag) : Component(RENDERER) {
    myModel = newModel;
    tag = newTag;
}

RendererComponent::~RendererComponent() = default;
