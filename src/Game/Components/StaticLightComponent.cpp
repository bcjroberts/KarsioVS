#include "StaticLightComponent.h"
#include "../../Engine/Core.h"


StaticLightComponent::StaticLightComponent(glm::vec3 position, glm::vec3 ncolor) : Component(STATIC_LIGHT) {
    myLightID = Core::renderEngine->world->getNextAvailableLightID();
    pos = position;
    color = ncolor;

    if (myLightID != -1) {
        Core::renderEngine->world->setLight(myLightID, pos, color);
    }
}

StaticLightComponent::~StaticLightComponent() {
    if (myLightID != -1) {
        Core::renderEngine->world->freeLightWithID(myLightID);
    }
}
