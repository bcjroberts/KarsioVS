//
// Created by BCJRo on 2018-01-10.
//

#ifndef PROJECTKARSIO_EVENT_H
#define PROJECTKARSIO_EVENT_H

enum EventType {COLLIDE, PLAYSOUND };

class Event {
public:
    EventType myEventType;
    Event(EventType aType);
    EventType getEventType();
    ~Event();
};


#endif //PROJECTKARSIO_EVENT_H
