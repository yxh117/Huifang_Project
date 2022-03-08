#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
    std:: unique_lock<std::mutex> uLock(_mutex); 
    //and _condition.wait() to wait for and receive new messages
    _condition.wait(uLock, [this] {return !_queue.empty();}); 
    //and pull them from the queue using move semantics. 
    T msg = std::move(_queue.front());
    _queue.pop_front();

    // The received object should then be returned by the receive function.
    return msg;

}

template <typename T>
void MessageQueue<T>::send(T &&message)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
    std::lock_guard<std::mutex> lck(_mutex);
    _queue.push_back(message);
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loo
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(bool flag = true)
    {
        if (message.receive() == TrafficLightPhase::green)
            flag = false;
    }
   
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    std::thread t(&TrafficLight::cycleThroughPhases, this);
    threads.emplace_back(std::move(t));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    std::uniform_int_distribution<> dist(4, 6);
    std::random_device rd;
    std::default_random_engine eng(rd());
    double Duration = dist(eng); //duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate; 
    //init
    lastUpdate = std::chrono::system_clock::now();

    while(true)
    {
        // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= Duration)
        {
            // toggle the current phase of the traffic light between red and green
            switch(_currentPhase){
                case TrafficLightPhase::red :
                    _currentPhase = TrafficLightPhase::green;
                 break;
                case TrafficLightPhase::green :
                    _currentPhase = TrafficLightPhase::red;
                     break;
            }

        }
        //reset stop watch for nex cycle
        lastUpdate = std::chrono::system_clock::now();
        //sends an update method to the message queue using move semantic
        message.send(std::move(_currentPhase));
        
    }

}

