
#include "myThread.h"
    void myThread::thread_workload() {
        while (true){
            Job j =jobQueue-> PCQueue<Job>::pop();
            if(j.is_exit){
                return ;
            }
            ///to do: pase 1
            (*j.counter1)++;
            while (*j.counter1!=j.total_jobs){
                pthread_cond_wait( cond1,m);
            }
            pthread_cond_broadcast(cond1);
            ///to do:: pase 2
            (*j.counter2)++;
            while (*j.counter2!=j.total_jobs){
                pthread_cond_wait( cond2,m);
            }
            pthread_cond_broadcast(cond2);

        }
    }

myThread::myThread(uint threadId, pthread_cond_t *cond1, pthread_cond_t *cond2, pthread_mutex_t *m,
                   PCQueue<Job> *jobQueue): Thread(threadId) {
    this->cond1=cond1;
    this->cond2=cond2;
    this->m=m;
    this->jobQueue=jobQueue;
}



