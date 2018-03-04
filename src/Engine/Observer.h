#pragma once
#include "Event.h"
// Abstract class. Future Observers should extend this class, as shown in AudioObserver
class Observer
{
public:
    virtual ~Observer();
    virtual void onNotify(Event anEvent) = 0;
};

