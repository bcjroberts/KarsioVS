#include "UpgradeComponent.h"
#include "../../Engine/Entity.h"
#include "HealthComponent.h"
#include "../../engine/EntityManager.h"


UpgradeComponent::UpgradeComponent() : Component(UPGRADE) {
}


UpgradeComponent::~UpgradeComponent()
{
}

void UpgradeComponent::addResources(float value) {
    resources += value;
}

float UpgradeComponent::getCurrentRamMultiplier() {
    return ramMultiplierChange[ramLevel - 1];
}

bool UpgradeComponent::isUpgradeAvailable() const {
    return resources >= resourcesForNextLevel;
}

bool UpgradeComponent::canUpgradeType(UpgradeType type) {
    switch (type) {
        case CHASSIS_UPGRADE:
            return (chassisLevel < 3 && numberOfUpgrades >= minUpgradesForChassisUpgrade[chassisLevel - 1]);
        case GUN_UPGRADE:
            return (gunLevel < maxLevelPerChassis[chassisLevel - 1]);
        case ARMOR_UPGRADE:
            return (armorLevel < maxLevelPerChassis[chassisLevel - 1]);
        case RAM_UPGRADE:
            return (ramLevel < maxLevelPerChassis[chassisLevel - 1]);
        default:
            return false;
    }
}

void UpgradeComponent::upgradeVehicle(UpgradeType type) {
    // Seubtract from our current resources, and update the next value
    resources -= resourcesForNextLevel;
    resourcesForNextLevel *= increasedResourceAmountModifier;

    if(!canUpgradeType(type)) return; // Ensure we can upgrade this thing first
    if (type == CHASSIS_UPGRADE) {
        chassisLevel++;
        static_cast<HealthComponent*>(owner->getComponent(HEALTH))->setMaxHealth(chassisHealth[chassisLevel-1], true);
        EntityManager::getInstance()->updateChassis(owner, 1.5f, chassisLevel);
    } else if (type == GUN_UPGRADE) {
        // Upgrade the stats of the gun component.
        gunLevel++;
        EntityManager::getInstance()->updateGun(owner, gunLevel);
    } else if (type == ARMOR_UPGRADE) {
        static_cast<HealthComponent*>(owner->getComponent(HEALTH))->setArmor(armorChange[armorLevel]);
        EntityManager::getInstance()->updateArmor(owner, chassisLevel, armorLevel);
    } else if (type == RAM_UPGRADE) {
        // Tell the entity manager to update the ram look.
        EntityManager::getInstance()->updateRam(owner, ramLevel);
    }
}

