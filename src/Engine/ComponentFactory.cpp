

#include "ComponentFactory.h"

ComponentFactory::ComponentFactory() {
}

ComponentFactory::~ComponentFactory() {
}

// Creates the component of the corresponding type and returns it.
Component* ComponentFactory::createComponent(ComponentType type) {
    switch (type)
    {
    case RENDERER:
        break;
    case PHYSICS:
        break;
    default:
        break;
    }
    Component c(RENDERER);
    return &c;
}