#pragma once
#include "Event.h"
class Observer
{
public:
    virtual ~Observer();
    virtual void onNotify(Event anEvent) = 0;
};

