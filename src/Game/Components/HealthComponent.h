#pragma once
#include "../../Engine/Component.h"

class HealthComponent : public Component
{
private:
    bool healthIsThreshold;
    float health;
	int hitID = -1;
public:
    HealthComponent(float nhealth, bool nhealthIsThreshold = false);
    bool isDead() const;
    void applyDamage(float damage);
    ~HealthComponent();
	int hitBy();
	void hit(int id = -1);	// if is hit, and by whom
};

