#include "ShapeRendererComponent.h"
#include <glm/gtx/quaternion.hpp>
#include <extensions/PxShapeExt.h>
#include "../../Engine/Entity.h"

ShapeRendererComponent::ShapeRendererComponent(ModelData* newModel,  physx::PxShape* newShape) : RendererComponent(newModel) {
    myShape = newShape;
}

mat4 ShapeRendererComponent::getMatrix()
{
    physx::PxTransform loc = myShape->getLocalPose();
                                                     
    //loc = physx::PxShapeExt::getGlobalPose(*myShape, *myShape->getActor());

    // copy the position of the shape
    position.x = loc.p.x;
    position.y = loc.p.y;
    position.z = loc.p.z;

    // copy the rotation from the shape
    rotation.w = loc.q.w;
    rotation.x = loc.q.x;
    rotation.y = loc.q.y;
    rotation.z = loc.q.z;

    mat4 myMat2;
    myMat2 = glm::translate(myMat2, position + localPos) * glm::toMat4(rotation) * glm::scale(myMat2, scale);
    return owner->getMatrix() * myMat2;
}



ShapeRendererComponent::~ShapeRendererComponent() = default;
