#include "AudioObservable.h"
#include <iostream>


AudioObservable::AudioObservable()
{
}


void AudioObservable::notifyObservers(AudioEvent anEvent) {
    for (int i = 0; i < AudioObservable::observerList.size(); i++) {
        AudioObservable::observerList.at(i)->onNotify(anEvent);
        std::cout << "Notifying observer number: " << i << " of sound request " << anEvent.soundfile << std::endl;
    }
}

void AudioObservable::notifyAudioObservers(AudioEvent anEvent) {
    std::cout << "Attempting to notify " << AudioObservable::audioObservers.size() << " observers of sound request " << anEvent.soundfile << std::endl;
    for (int i = 0; i < AudioObservable::audioObservers.size(); i++) {
        AudioObservable::audioObservers.at(i)->onAudioNotify(anEvent);
        std::cout << "Notified observer number: " << i << " of " << AudioObservable::audioObservers.size() << std::endl;
    }
}

void AudioObservable::addAudioObserver(AudioObserver* anObserver) {
    std::cout << "adding observer: " << anObserver << std::endl;
    audioObservers.push_back(anObserver);
}

void AudioObservable::remAudioObserver(AudioObserver* anObserver) {
    audioObservers.erase(std::find(audioObservers.begin(), audioObservers.end(), anObserver));
}



AudioObservable::~AudioObservable()
{
}
