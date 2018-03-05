#pragma once
#include "../../Engine/Component.h"

class ProjectileComponent : public Component{
private:
    int ownerid;
    float speed;
    float damage;
    float distanceToCheck;
public:
    ProjectileComponent(int nownerid, float nspeed, float ndamage);
    ~ProjectileComponent();
    bool checkForHit();
    void move();
};

