#include "RendererComponent.h"



RendererComponent::RendererComponent() : Component(RENDERER) {
	eventBitmask = 0;
}

RendererComponent::~RendererComponent() {

}

int RendererComponent::getEventBitmask() {
	return eventBitmask;
}
