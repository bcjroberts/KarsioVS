#include "FloatingTextComponent.h"
#include "../../Engine/Core.h"
#include "../../Engine/Entity.h"


FloatingTextComponent::FloatingTextComponent(Entity* relativeEnt, glm::vec3 color, float scale, std::string* text) : Component(FLOATING_TEXT) {
    // Create text at the location where this is spawned
    myRelativeEnt = relativeEnt;
    glm::vec2 tempScreenLoc = Core::cameras[0]->worldToScreenPoint(relativeEnt->getPosition());
    myTextId = Core::renderEngine->ui->addText(*text, tempScreenLoc.x, tempScreenLoc.y, scale, color, 1);
    simCreationTime = Core::simtimeSinceStartup;
}

void FloatingTextComponent::updateTextPosition(float timeDiff) {
    glm::vec3 oldPos = owner->getPosition();
    if (myRelativeEnt != nullptr) { // move to position of the owner entity
        glm::vec3 temp = myRelativeEnt->getPosition();
        oldPos.x = temp.x;
        oldPos.z = temp.z;
    }
    owner->updatePosition(oldPos + glm::vec3(0, 3.f * timeDiff, 0));

    glm::vec2 tempScreenLoc = Core::cameras[0]->worldToScreenPoint(owner->getPosition());
    Core::renderEngine->ui->modifyText(myTextId, nullptr, &tempScreenLoc.x, &tempScreenLoc.y);
}

FloatingTextComponent::~FloatingTextComponent() {
    Core::renderEngine->ui->removeText(myTextId);
}
