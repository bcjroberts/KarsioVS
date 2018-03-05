#pragma once
#include "../../Engine/Component.h"

class HealthComponent : public Component
{
private:
    bool healthIsThreshold;
    float health;
	int hitID = -1;
    float maxHealth;
    float armor = 0;
public:
    HealthComponent(float nhealth, bool nhealthIsThreshold = false, float narmor = 0);
    bool isDead() const;
    void applyDamage(float damage);
    float getMaxHealth() const { return maxHealth; }
    ~HealthComponent();
	int hitBy();
	void hit(int id = -1);	// if is hit, and by whom
    void setArmor(float narmor);
    void setMaxHealth(float nmaxHealth, bool setHealth = false);
    float getCurrentHealth() {return health; }
};

