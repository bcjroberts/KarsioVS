#include "RendererComponent.h"



RendererComponent::RendererComponent() : Component(RENDERER) {
	myMesh = nullptr;
	myShader = nullptr;
}

RendererComponent::~RendererComponent() = default;
