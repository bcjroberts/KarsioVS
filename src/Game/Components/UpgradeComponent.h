#pragma once
#include "../../Engine/Component.h"

class UpgradeComponent : public Component
{
private:
    float resources;
public:
    UpgradeComponent();
    ~UpgradeComponent();
    void addResources(float value);
};

