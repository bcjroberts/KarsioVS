#include "RendererComponent.h"



RendererComponent::RendererComponent() : Component(RENDERER) {
	eventBitmask = 0;
	myMesh = nullptr;
	myShader = nullptr;
}

RendererComponent::~RendererComponent() = default;

int RendererComponent::getEventBitmask() {
	return eventBitmask;
}

void RendererComponent::processFrameUpdate() {
	// printf("Renderer component processing frame!");
}
