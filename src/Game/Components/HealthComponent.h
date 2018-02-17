#pragma once
#include "../../Engine/Component.h"

class HealthComponent : public Component
{
private:
    bool healthIsThreshold;
    float health;
public:
    HealthComponent(float nhealth, bool nhealthIsThreshold = false);
    bool isDead() const;
    void applyDamage(float damage);
    ~HealthComponent();
};

