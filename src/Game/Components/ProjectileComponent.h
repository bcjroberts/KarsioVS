#pragma once
#include "../../Engine/Component.h"

class ProjectileComponent : public Component{
private:
    int ownerid;
    float speed;
    float damage;
    float distanceToCheck;
    float previousSimTime;
    float explosionStartTime;
public:
    ProjectileComponent(int nownerid, float nspeed, float ndamage);
    ~ProjectileComponent();
    bool checkForHit();
    bool shouldDestroyExplosion();
    void move();
    bool hitSomething = false;
};

