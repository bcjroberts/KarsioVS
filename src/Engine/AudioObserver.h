#pragma once
#include "Observer.h"

class AudioObserver : public Observer
{
public:
    virtual void onNotify(Event anEvent);
};