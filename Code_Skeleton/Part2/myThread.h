//
// Created by student on 1/5/21.
//

#ifndef HW3_MYTHREAD_H
#define HW3_MYTHREAD_H
#include "Thread.hpp"
#include "Job.h"
#include "../Part1/PCQueue.hpp"

class myThread : public Thread {
    pthread_cond_t *cond1;
    pthread_cond_t *cond2;
    pthread_mutex_t *m;
    PCQueue <Job> *jobQueue;
    vector<double>* m_tile_hist;
public:
    myThread(uint threadId, pthread_cond_t *cond1, pthread_cond_t *cond2, pthread_mutex_t *m,
             PCQueue<Job> *jobQueue, vector<double>* m_tile_hist);
    void thread_workload() override;
};


#endif //HW3_MYTHREAD_H
