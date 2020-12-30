#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "Code_Skeleton/Part1/Headers.hpp"
#include "Code_Skeleton/Part1/Semaphore.hpp"

Semaphore* semaphore;

void* thread(void* arg)
{
    //wait
    semaphore->down();
    printf("\nEntered..\n");

    //critical section
    sleep(4);

    //signal
    printf("\nJust Exiting...\n");
    semaphore->up();
}


int main()
{
//    sem_init(&mutex, 0, 1);
    semaphore= new Semaphore(1);
    pthread_t t1,t2;
    pthread_create(&t1,NULL,thread,NULL);
    sleep(2);
    pthread_create(&t2,NULL,thread,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
//    sem_destroy(&mutex);
    return 0;
}