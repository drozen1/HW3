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
    int x=my_queue.pop();
    cout<<x<<endl;
}

void* my_push(void* arg){
    int x= *((int*)arg);
    cout<<"sucess to push "<< x<<endl;
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
    pthread_t c1,c2,p1;
    pthread_create(&c1,NULL,my_pop,NULL);
    pthread_create(&c2,NULL,my_pop,NULL);
    pthread_create(&p1,NULL,my_push,&x1);
    pthread_create(&p1,NULL,my_push,&x2);

    return 0;
}