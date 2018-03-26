#pragma once
#include "Observer.h"
#include "queue"
#include "AudioEvent.h"
#include "Event.h"
#include "NullEvent.h"

class AudioObserver : public Observer
{
private:
    std::queue<AudioEvent> audioQueue;
public:
    virtual void onNotify(Event anEvent);
    void onNotify(AudioEvent anEvent);
    AudioEvent nextAudioEvent();
    bool isEmpty();
    AudioObserver();
    ~AudioObserver();
};