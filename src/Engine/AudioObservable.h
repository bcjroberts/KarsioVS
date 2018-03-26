#pragma once
#include "Event.h"
#include "AudioEvent.h"
#include "Observable.h"
class AudioObservable : public Observable
{
public:
    void notifyObservers(AudioEvent Event);
    AudioObservable();
    ~AudioObservable();
};

