#pragma once
#include "Event.h"
#include "AudioEvent.h"
#include "Observable.h"
#include "AudioObserver.h"
class AudioObservable : public Observable
{

public:
    void notifyObservers(AudioEvent Event);
    void notifyAudioObservers(AudioEvent Event);
    void addAudioObserver(AudioObserver* anObserver);
    void remAudioObserver(AudioObserver* anObserver);
    AudioObservable();
    ~AudioObservable();
    std::vector<AudioObserver*> audioObservers;
};

