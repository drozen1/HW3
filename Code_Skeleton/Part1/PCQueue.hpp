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
	T pop() {
	    pthread_mutex_lock(&global_lock);
        //printf("POP: writers_inside: %d , readers_inside: %d,queue.size= %d, readers_inside= %d   \n",writers_inside,readers_inside,this->queue.size(),readers_inside);
        while(writers_inside> 0 || writers_waiting>0 || this->queue.size()==0 || readers_inside>0){
            pthread_cond_wait(&read_allowed, &global_lock);
        }
        readers_inside++;
        T* ret= &this->queue.front();
//        cout<<"pop val is: "<<*ret<<endl;
        this->queue.pop();
        ///DEBUG

//        cout<<"pop val is: "<<*ret<<endl;
        readers_inside--;
        pthread_cond_signal(&write_allowed);
        if(writers_waiting == 0)
            pthread_cond_signal(&read_allowed);

        //printf("pop val is: %d \n",*ret);
        pthread_mutex_unlock(&global_lock);
        return *ret;
	}

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item) {  pthread_mutex_lock(&global_lock);
        writers_waiting++;
        //printf("PUSH: writers_inside: %d , readers_inside: %d  \n",writers_inside,readers_inside);
        while(writers_inside+readers_inside> 0 ){
            pthread_cond_wait(&write_allowed, &global_lock);
        }
        writers_inside++;
        this->queue.push(item);
        writers_inside--;
        writers_waiting--;
        ///DEBUG
        //cout<<"sucess to push "<< item<<endl;
        //printf("sucess to push: %d \n",item);
        pthread_cond_signal(&write_allowed);
        pthread_cond_signal(&read_allowed);
        pthread_mutex_unlock(&global_lock);
	}

	PCQueue() {
        readers_inside = 0;
        writers_inside = 0;
        writers_waiting = 0;
        int ret = pthread_cond_init(&this->read_allowed, NULL);
        if (ret != 0) {
            cout << "BUGG: cont_init fail" << endl;
        }
        ret = pthread_cond_init(&this->write_allowed, NULL);
        if (ret != 0) {
            cout << "BUGG: cont_init fail" << endl;
        }
        ret = pthread_mutex_init(&global_lock, NULL);
        if (ret != 0) {
            cout << "BUGG: mutex_init fail" << endl;
        }
    }

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