#pragma once

#include "RendererComponent.h"
#include <PxShape.h>

class ShapeRendererComponent : public RendererComponent
{
private:
    physx::PxShape* myShape;
public:
    vec3 localPos;
    ShapeRendererComponent(ModelData* newModel, physx::PxShape* newShape);
    mat4 getMatrix() override;
    ~ShapeRendererComponent();
};

