#include "HealthComponent.h"
#include "../../Engine/EntityManager.h"
#include "../../Engine/Entity.h"
#include "AIComponent.h"
#include "../../Engine/Core.h"


HealthComponent::HealthComponent(float nhealth, bool nhealthIsThreshold, float narmor) : Component(HEALTH){
    health = maxHealth = nhealth;
    healthIsThreshold = nhealthIsThreshold;
    armor = narmor;
}

bool HealthComponent::isDead() const {
    return health <= 0;
}

bool HealthComponent::isPlayerInvolved(int id) {
    if (Core::playerVehicle == nullptr) return false;
    else if (Core::playerVehicle->id == owner->id || Core::playerVehicle->id == id) {
        return true;
    }
    return false;
}

void HealthComponent::applyDamage(float damage, int damageSenderId) {
    // A basic implementation of armor. Could be chanegd to be a fraction rather than a subtraction, but maybe not.
    damage -= armor;
    damage = damage < 0 ? 0 : damage;

    // let AI know who hit it
    if (damage > 0 && owner->getComponent(AI) != nullptr) {
        static_cast<AIComponent*>(owner->getComponent(AI))->setAttackerID(damageSenderId);
    }

    // This is for crystals which are only destroyed if they are hit above a certain threshold
    if (healthIsThreshold && damage > health) {
        health = 0;
    } else if (!healthIsThreshold){
        health -= damage;

        // Print the rounded damage
        if (isPlayerInvolved(damageSenderId)) {
            const int roundedDamage = round(damage);
            std::string damageString = std::to_string(roundedDamage);
            glm::vec3 color(1,1,1);
            float scale = 0.5f;
            if (damage > 0) color = glm::vec3(1,1,0);
            if (damage >= 4.5f) {
                color = glm::vec3(1, 0.7, 0);
                scale = 0.6f;
            }
            if (damage >= 10.f) {
                color = glm::vec3(1, 0.5, 0);
                scale = 0.75f;
            }
            EntityManager::getInstance()->createFloatingText(owner, color, scale, &damageString);
        }

        // Lock health to 0
        health = health < 0 ? 0 : health;
    }
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

void HealthComponent::addHealth(float x) {

    if (isPlayerInvolved(-1)) {
        const int roundedHealth = round(x);
        std::string healthString = std::to_string(roundedHealth);
        EntityManager::getInstance()->createFloatingText(owner, glm::vec3(0,1,0), 0.6f, &healthString);
    }

    health += x;
    health = health > maxHealth ? maxHealth : health;
}

int HealthComponent::hitBy() {
	return hitID;
}

HealthComponent::~HealthComponent() = default;

