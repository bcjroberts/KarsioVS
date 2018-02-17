#include "HealthComponent.h"



HealthComponent::HealthComponent(float nhealth, bool nhealthIsThreshold) : Component(HEALTH){
    health = nhealth;
    healthIsThreshold = nhealthIsThreshold;
}

bool HealthComponent::isDead() const {
    return health <= 0;
}

void HealthComponent::applyDamage(float damage) {
    // This is for crystals which are only destroyed if they are hit above a certain threshold
    if (healthIsThreshold && damage < health)
        return;
    
    health -= damage;
}

HealthComponent::~HealthComponent()
{
}
