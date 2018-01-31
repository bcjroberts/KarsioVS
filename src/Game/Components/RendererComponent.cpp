#include "RendererComponent.h"
#include <glm/gtx/quaternion.hpp>
#include "../../Engine/Entity.h"

mat4 RendererComponent::getMatrix() {
    mat4 myMatrix = glm::translate(glm::scale(glm::toMat4(rotation),scale),position);
    mat4 ownerMatrix = owner->getMatrix();
    return ownerMatrix * myMatrix;
}

RendererComponent::RendererComponent(MeshData* newMesh, ShaderData* newShader) : Component(RENDERER)
{
    myMesh = newMesh;
    myShader = newShader;
}

RendererComponent::~RendererComponent() = default;
