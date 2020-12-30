#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include <queue>

// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue

{

public:
	// Blocks while queue is empty.
	// When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop();

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item); 

	PCQueue();

private:
	// Add your class memebers here
	std::queue<T> queue;
    pthread_mutex_t global_lock;
    pthread_cond_t read_allowed;
    pthread_cond_t write_allowed;
    int readers_inside, writers_inside, writers_waiting;
};


// Recommendation: Use the implementation of the std::queue for this exercise
#endif