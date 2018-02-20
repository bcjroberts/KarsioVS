//
// Created by BCJRo on 2018-01-10.
//

#include "Event.h"
Event::Event(EventType atype) {
    myEventType = atype;
}
EventType Event::getEventType() {
    return myEventType;
}
Event::~Event() {

}