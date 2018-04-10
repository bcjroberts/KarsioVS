#pragma once
#include "../../Engine/Component.h"
#include "ShapeRendererComponent.h"

class WeaponComponent : public Component{
private:
    float lastTimeFired = 0.f;
    float gunROF;
    float timeBetweenShots;
    float gunDamage;
    float projectileSpeed;
    ShapeRendererComponent* myWeapon;
    Entity* targetEnt = nullptr;
    const float maxGunPitch = 1.f;
    const float maxGunYaw = 1.f;
public:
    WeaponComponent(ShapeRendererComponent* newWeapon);
    ~WeaponComponent();
    void fireWeapon();
    void setROF(float ROF);
    void updateGunValues(float nROF, float newDamaga, float newSpeed);
    void updateTargetting();
    void updateTarget(Entity* newTarget);
};

