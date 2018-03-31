#include "AudioObserver.h"
#include <iostream>

void AudioObserver::onNotify(Event anEvent) {
        std::cout << "Did not receive request for " + anEvent.getEventType() << std::endl;
}

void AudioObserver::onAudioNotify(AudioEvent anEvent) {
        //std::cout << "Received request for " + anEvent.soundfile << std::endl;
        //std::cout << "Queue size before push = " << audioQueue.size() << std::endl;
        audioQueue.push(anEvent); // push event if it is a sound event
        //std::cout << "Queue size after push = " << audioQueue.size() << std::endl;
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


