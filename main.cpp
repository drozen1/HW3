#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "Code_Skeleton/Part1/Headers.hpp"
#include "Code_Skeleton/Part1/Semaphore.hpp"
#include "Code_Skeleton/Part1/PCQueue.hpp"

PCQueue<int> my_queue;
//Semaphore* semaphore;

//void* thread(void* arg)
//{
//    //wait
//    semaphore->down();
//    printf("\nEntered..\n");
//
//    //critical section
//    sleep(4);
//
//    //signal
//    printf("\nJust Exiting...\n");
//    semaphore->up();
//}

void* my_pop(void* arg){
    my_queue.pop();
}

void* my_push(void* arg){
    int x= *((int*)arg);

    my_queue.push(x);
}


int main()
{
//    semaphore= new Semaphore(1);
//    pthread_t t1,t2;
//    pthread_create(&t1,NULL,thread,NULL);
//    sleep(2);
//    pthread_create(&t2,NULL,thread,NULL);
//    pthread_join(t1,NULL);
//    pthread_join(t2,NULL);
    my_queue= PCQueue<int>();
    int x1=1, x2=2, x3=3;
    pthread_t c1,c2,c3,c4,p1;

    //sleep(1);

    pthread_create(&c1,NULL,my_pop,NULL);
    sleep(1);
    pthread_create(&c2,NULL,my_pop,NULL);
    sleep(1);
    pthread_create(&c3,NULL,my_pop,NULL);
    sleep(1);
    pthread_create(&c4,NULL,my_pop,NULL);
    //sleep(1);

    pthread_create(&p1,NULL,my_push,&x1);
    //sleep(1);
    pthread_create(&p1,NULL,my_push,&x2);
    //sleep(1);
    pthread_create(&p1,NULL,my_push,&x3);
    sleep(1);

    pthread_join(p1,NULL);
    pthread_join(c1,NULL);
    pthread_join(c2,NULL);
    pthread_join(c3,NULL);
    pthread_join(c4,NULL);


    return 0;
}