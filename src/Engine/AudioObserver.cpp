#include "AudioObserver.h"

void AudioObserver::onNotify(Event anEvent) {
    if (anEvent.getEventType() == PLAYSOUND) {
        audioQueue.push(anEvent); // push event if it is a sound event
    }
    // ignore otherwise
}

Event AudioObserver::nextAudioEvent() {
    if (AudioObserver::audioQueue.empty()) {
        NullEvent TestEvent = NullEvent();
        return TestEvent; // fire null event if queue is empty.
    }
    else{
        Event tempEvent = AudioObserver::audioQueue.front();
        audioQueue.pop();
        return tempEvent;
    }
}

bool AudioObserver::isEmpty() {
    return AudioObserver::audioQueue.empty();
}

AudioObserver::AudioObserver() {
}
AudioObserver::~AudioObserver() = default;


