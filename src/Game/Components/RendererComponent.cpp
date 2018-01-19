#include "RendererComponent.h"
#include <glm/gtx/quaternion.hpp>

mat4 RendererComponent::getMatrix() const {
    mat4 myMatrix = glm::translate(glm::scale(glm::toMat4(rotation),scale),position);
    return myMatrix;
}

RendererComponent::RendererComponent(MeshData* newMesh, ShaderData* newShader) : Component(RENDERER)
{
    myMesh = newMesh;
    myShader = newShader;
}

RendererComponent::~RendererComponent() = default;
