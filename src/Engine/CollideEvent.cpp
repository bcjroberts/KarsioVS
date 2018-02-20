#include "CollideEvent.h"

CollideEvent::CollideEvent(int newEnt1ID, int newEnt2ID) : Event(COLLIDE){
    ent1ID = newEnt1ID;
    ent2ID = newEnt2ID;
}

CollideEvent::~CollideEvent()
{
}
