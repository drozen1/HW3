//
// Created by student on 1/5/21.
//
#include "Thread.hpp"
#include "Job.h"
#include "../Part1/PCQueue.hpp"

class myThread : public Thread {
    pthread_cond_t *cond1;
    pthread_cond_t *cond2;
    pthread_mutex_t *m;
    PCQueue<Job>* jobQueue;

    void thread_workload() override {
        while (true){
            Job j =jobQueue->pop();
            if(j.is_exit){
                return ;
            }
            //to do: pase 1
            (*j.counter1)++;
            while (*j.counter1!=j.total_jobs){
                pthread_cond_wait( cond1,m);
            }
            pthread_cond_broadcast(cond1);
            //to do:: pase 2
            (*j.counter2)++;
            while (*j.counter2!=j.total_jobs){
                pthread_cond_wait( cond2,m);
            }
            pthread_cond_broadcast(cond2);
            //print the bord

        }
    }
}