#include "AudioObserver.h"

void AudioObserver::onNotify(Event anEvent) {
    if (anEvent.getEventType() == PLAYSOUND) {
        //
    }
}

void AudioObserver::onNotify(AudioEvent anEvent) {
    if (anEvent.getEventType() == PLAYSOUND) {
        audioQueue.push(anEvent); // push event if it is a sound event
    }
    // ignore otherwise
}

AudioEvent AudioObserver::nextAudioEvent() {
    if (!AudioObserver::audioQueue.empty()) {
        AudioEvent nextEvent = AudioObserver::audioQueue.front();
        audioQueue.pop();
        return nextEvent;
    }
    else{
        AudioEvent blankEvent = AudioEvent(glm::vec3(0,0,0), "");
        return blankEvent;
    }        
}

bool AudioObserver::isEmpty() {
    return AudioObserver::audioQueue.empty();
}

AudioObserver::AudioObserver() {
}


AudioObserver::~AudioObserver() = default;


