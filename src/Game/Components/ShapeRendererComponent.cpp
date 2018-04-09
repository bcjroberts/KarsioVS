#include "ShapeRendererComponent.h"
#include <glm/gtx/quaternion.hpp>
#include <extensions/PxShapeExt.h>
#include "../../Engine/Entity.h"

//using namespace glm;
ShapeRendererComponent::ShapeRendererComponent(Model* newModel,  physx::PxShape* newShape, RendererTag newTag) : RendererComponent(newModel, newTag) {
    myShape = newShape;
}

glm::mat4 ShapeRendererComponent::getMatrix()
{
    physx::PxTransform loc = myShape->getLocalPose();

    // copy the position of the shape
    position.x = loc.p.x;
    position.y = loc.p.y;
    position.z = loc.p.z;

    // copy the rotation from the shape
    glm::quat newRot(eulerAngles(rotation) + glm::eulerAngles(glm::quat(loc.q.w, loc.q.x, loc.q.y, loc.q.z)));

	glm::mat4 myMat2;
    myMat2 = glm::translate(myMat2, position + localPos) * glm::toMat4(newRot) * glm::scale(myMat2, scale);
    return owner->getMatrix() * myMat2;
}



ShapeRendererComponent::~ShapeRendererComponent() = default;
