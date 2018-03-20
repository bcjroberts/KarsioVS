#pragma once
#include "Observer.h"
#include "queue"
#include "AudioEvent.h"
#include "Event.h"
#include "NullEvent.h"

class AudioObserver : public Observer
{
private:
    std::queue<Event> audioQueue;
public:
    virtual void onNotify(Event anEvent);
    Event nextAudioEvent();
    bool isEmpty();
    AudioObserver();
    ~AudioObserver();
};