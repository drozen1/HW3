//
// Created by student on 12/29/20.
//

#include "Semaphore.hpp"

Semaphore::Semaphore() {

    int ret;
/* initialize a condition variable to its default value */
    ret = pthread_cond_init(&this->cv, NULL);
    if (ret!=0){
        cout<<"BUGG: cont_init fail"<<endl;
    }
    this->counter=0;
}

Semaphore::Semaphore(unsigned int val) {
    int ret;
/* initialize a condition variable to its default value */
    ret = pthread_cond_init(&this->cv, NULL);
    if (ret!=0){
        cout<<"BUGG: cont_init fail"<<endl;
    }
    this->counter=val;
}

//signal
void Semaphore::up() {
    pthread_mutex_lock(&m);
    counter++;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&m);
}

//wait
void Semaphore::down() {
    pthread_mutex_lock(&m);
    while(counter==0){
        pthread_cond_wait(&cv,&m);
    }
    counter--;
    pthread_mutex_unlock(&m);
}
