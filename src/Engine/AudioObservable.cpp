#include "AudioObservable.h"



AudioObservable::AudioObservable()
{
}


void AudioObservable::notifyObservers(AudioEvent anEvent) {
    for (int i = 0; i < AudioObservable::observerList.size(); i++) {
        AudioObservable::observerList.at(i)->onNotify(anEvent);
    }
}

AudioObservable::~AudioObservable()
{
}
