#include "UpgradeComponent.h"



UpgradeComponent::UpgradeComponent() : Component(UPGRADE) {
}


UpgradeComponent::~UpgradeComponent()
{
}

void UpgradeComponent::addResources(float value) {
    resources += value;
}
