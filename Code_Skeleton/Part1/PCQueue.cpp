//
// Created by student on 12/30/20.
//

#include "PCQueue.hpp"
// Blocks while queue is empty.
// When queue holds items, allows for a single
// thread to enter and remove an item from the front of the queue and return it.
// Assumes multiple consumers.
template<typename T>
T PCQueue<T>::pop() {
    pthread_mutex_lock(&global_lock);
    while(writers_inside> 0 || writers_waiting>0 || this->queue.size()>0 || readers_inside>0){
        pthread_cond_wait(&read_allowed, &global_lock);
    }
    readers_inside++;
    T* ret= &this->queue.front();
    this->queue.pop();
    readers_inside--;
    pthread_cond_signal(&write_allowed);
    if(writers_waiting == 0)
        pthread_cond_signal(&read_allowed);
    pthread_mutex_unlock(&global_lock);
    return ret;
}



// Allows for producer to enter with *minimal delay* and push items to back of the queue.
// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
// Assumes single producer
template<typename T>
void PCQueue<T>::push(const T &item) {
    pthread_mutex_lock(&global_lock);
    writers_waiting++;
    while(writers_inside+readers_inside> 0 ){
        pthread_cond_wait(&write_allowed, &global_lock);
    }
    writers_inside++;
    this->queue.push(item);
    writers_inside--;
    writers_waiting--;
    pthread_cond_signal(&write_allowed);
    pthread_cond_signal(&read_allowed);
    pthread_mutex_unlock(&global_lock);
}

template<typename T>
PCQueue<T>::PCQueue() {
    readers_inside = 0;
    writers_inside = 0;
    writers_waiting = 0;
    int ret = pthread_cond_init(&this->read_allowed, NULL);
    if (ret!=0){
        cout<<"BUGG: cont_init fail"<<endl;
    }
    ret = pthread_cond_init(&this->write_allowed, NULL);
    if (ret!=0){
        cout<<"BUGG: cont_init fail"<<endl;
    }
    ret = pthread_mutex_init(&global_lock, NULL);
    if (ret!=0){
        cout<<"BUGG: mutex_init fail"<<endl;
    }

}
