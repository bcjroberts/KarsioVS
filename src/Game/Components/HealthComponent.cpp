#include "HealthComponent.h"



HealthComponent::HealthComponent(float nhealth, bool nhealthIsThreshold, float narmor) : Component(HEALTH){
    health = maxHealth = nhealth;
    healthIsThreshold = nhealthIsThreshold;
    armor = narmor;
}

bool HealthComponent::isDead() const {
    return health <= 0;
}

void HealthComponent::applyDamage(float damage) {
    // A basic implementation of armor. Could be chanegd to be a fraction rather than a subtraction, but maybe not.
    damage -= armor;

    // This is for crystals which are only destroyed if they are hit above a certain threshold
    if (healthIsThreshold && damage < health)
        return;
    
    health -= damage;
}

// simply to check if collision on it detected (but by who?)
void HealthComponent::hit(int id) {
	hitID = id;
}

void HealthComponent::setArmor(float narmor) {
    armor = narmor;
}

void HealthComponent::setMaxHealth(float nmaxHealth, bool setHealth) {
    maxHealth = nmaxHealth;
    if (setHealth) health = maxHealth;
}

bool HealthComponent::isDeathProcessed() {
    if (!deathProcessed) {
        deathProcessed = true;
        return false;
    }
    return true;
}

int HealthComponent::hitBy() {
	return hitID;
}

HealthComponent::~HealthComponent() = default;

