
#include "Observable.h"

Observable::Observable()
{
}

void Observable::addObserver(Observer* anObserver) {
    observerList.push_back(anObserver);
}

void Observable::remObserver(Observer* anObserver) {
    observerList.erase(std::find(observerList.begin(),observerList.end(),anObserver));
}

void Observable::notifyObservers(Event anEvent) {
    for (int i=0; i < observerList.size(); i++) {
        observerList.at(i)->onNotify(anEvent);
    }
}

Observable::~Observable()
{
}
