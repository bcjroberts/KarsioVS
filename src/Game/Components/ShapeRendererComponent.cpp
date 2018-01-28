#include "ShapeRendererComponent.h"
#include <glm/gtx/quaternion.hpp>
#include <extensions/PxShapeExt.h>

ShapeRendererComponent::ShapeRendererComponent(MeshData* newMesh, ShaderData* newShader, physx::PxShape* newShape) : RendererComponent(newMesh, newShader)
{
    myShape = newShape;
}

mat4 ShapeRendererComponent::getMatrix()
{
    physx::PxTransform loc = myShape->getLocalPose();//physx::PxShapeExt::getGlobalPose(*myShape, *myActor);
                                                     // copy the position from the shape
    loc = physx::PxShapeExt::getGlobalPose(*myShape, *myShape->getActor());

    position.x = loc.p.x;
    position.y = loc.p.y;
    position.z = loc.p.z;

    // copy the rotation from the shape
    rotation.w = loc.q.w;
    rotation.x = loc.q.x;
    rotation.y = loc.q.y;
    rotation.z = loc.q.z;

    mat4 myMat2;
    myMat2 = glm::translate(myMat2, position) * glm::toMat4(rotation) * glm::scale(myMat2, scale);
    mat4 myMatrix = glm::translate(glm::scale(glm::toMat4(rotation), scale), position);
    return myMat2;
}



ShapeRendererComponent::~ShapeRendererComponent() = default;
