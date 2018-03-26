#pragma once
#include "Observer.h"
#include "queue"
#include "AudioEvent.h"
#include "Event.h"
#include "NullEvent.h"

class AudioObserver : public Observer
{
public:
    std::queue<AudioEvent> audioQueue;
    virtual void onNotify(Event anEvent);
    void onAudioNotify(AudioEvent anEvent);
    AudioEvent nextAudioEvent();
    bool isEmpty();
    AudioObserver();
    ~AudioObserver();
};