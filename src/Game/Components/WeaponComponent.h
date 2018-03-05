#pragma once
#include "../../Engine/Component.h"

class WeaponComponent : public Component{
private:
    float lastTimeFired = 0.f;
    float gunROF;
    float timeBetweenShots;
    float gunDamage;
    float projectileSpeed;
public:
    WeaponComponent();
    ~WeaponComponent();
    void fireWeapon();
    void setROF(float ROF);
    void updateGunValues(float nROF, float newDamaga, float newSpeed);
};

