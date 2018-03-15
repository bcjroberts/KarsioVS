#include "StaticLightComponent.h"
#include "../../Engine/Core.h"


StaticLightComponent::StaticLightComponent(glm::vec3 position, glm::vec3 ncolor) : Component(STATIC_LIGHT) {
    myLightID = Core::renderEngine->getNextAvailableLightID();
    pos = position;
    color = ncolor;

    if (myLightID != -1) {
        Core::renderEngine->setLight(myLightID, pos, color);
    }
}

StaticLightComponent::~StaticLightComponent() {
    if (myLightID != -1) {
        Core::renderEngine->freeLightWithID(myLightID);
    }
}
