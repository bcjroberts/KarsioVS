#pragma once

#include "RendererComponent.h"
#include <PxShape.h>

class ShapeRendererComponent : public RendererComponent
{
private:
    physx::PxShape* myShape;
    physx::PxRigidActor* myActor;
public:
    ShapeRendererComponent(MeshData* newMesh, ShaderData* newShader, physx::PxShape* newShape, physx::PxRigidActor* newActor);
    mat4 getMatrix() override;
    ~ShapeRendererComponent();
};

