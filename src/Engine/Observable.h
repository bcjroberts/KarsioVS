#pragma once
#include "Event.h"
#include "vector"
#include "Observer.h"
class Observable
{
public:
    Observable();
    void addObserver(Observer* anObserver);
    void remObserver(Observer* anObserver);
    void notifyObservers(Event anEvent);
    ~Observable();
private:
    std::vector<Observer*> observerList;
};

