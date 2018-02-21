#pragma once
#include "Event.h"
#include "Entity.h"
class CollideEvent : public Event
{
public:
    int ent1ID, ent2ID;
    CollideEvent(int newEnt1ID, int newEnt2ID);
    ~CollideEvent();
};

