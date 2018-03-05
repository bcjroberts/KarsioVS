#include "UpgradeComponent.h"
#include "../../Engine/Entity.h"
#include "HealthComponent.h"
#include "../../engine/EntityManager.h"
#include "WeaponComponent.h"


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

bool UpgradeComponent::upgradeVehicle(UpgradeType type) {
    // Seubtract from our current resources, and update the next value
    //if (!isUpgradeAvailable()) return false;

    resources -= resourcesForNextLevel;
    resourcesForNextLevel *= increasedResourceAmountModifier;
    numberOfUpgrades++;

    if(!canUpgradeType(type)) return false; // Ensure we can upgrade this thing first
    if (type == CHASSIS_UPGRADE) {
        chassisLevel++;
        static_cast<HealthComponent*>(owner->getComponent(HEALTH))->setMaxHealth(chassisHealth[chassisLevel-1], true);
        EntityManager::getInstance()->updateChassis(owner, 1.5f, chassisLevel);
        EntityManager::getInstance()->updateArmor(owner, chassisLevel, armorLevel);
        return true;
    } else if (type == GUN_UPGRADE) {
        // Upgrade the stats of the gun component.
        gunLevel++;
        static_cast<WeaponComponent*>(owner->getComponent(WEAPON))->updateGunValues(gunROFChange[gunLevel-1], gunDamageChange[gunLevel-1], projectileSpeedChange[gunLevel-1]);
        EntityManager::getInstance()->updateGun(owner, gunLevel);
        return true;
    } else if (type == ARMOR_UPGRADE) {
        armorLevel++;
        static_cast<HealthComponent*>(owner->getComponent(HEALTH))->setArmor(armorChange[armorLevel-1]);
        EntityManager::getInstance()->updateArmor(owner, chassisLevel, armorLevel);
        return true;
    } else if (type == RAM_UPGRADE) {
        // Tell the entity manager to update the ram look.
        ramLevel++;
        EntityManager::getInstance()->updateRam(owner, ramLevel);
        return true;
    }
    return false;
}

