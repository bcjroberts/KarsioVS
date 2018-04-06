#pragma once
#include "../../Engine/Component.h"

class HealthComponent : public Component
{
private:
    bool healthIsThreshold;
    bool deathProcessed = false;
    float health;
	int hitID = -1;
    float maxHealth;
    float armor = 0;
    bool HealthComponent::isPlayerInvolved(int id);
public:
    HealthComponent(float nhealth, bool nhealthIsThreshold = false, float narmor = 0);
    bool isDead() const;
    void applyDamage(float damage, int damageSenderId);
    float getMaxHealth() const { return maxHealth; }
    ~HealthComponent();
	int hitBy();
	void hit(int id = -1);	// if is hit, and by whom
    void setArmor(float narmor);
    void setMaxHealth(float nmaxHealth, bool setHealth = false);
    float getCurrentHealth() {return health; }
    bool isDeathProcessed();
    void addHealth(float x);
};

