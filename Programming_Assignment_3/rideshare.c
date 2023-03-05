#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

sem_t critSec, waitingA, waitingB;
int sleepingA = 0, sleepingB = 0;
pthread_barrier_t driverBar;

void* SupporterA(void * args){
    int  IsDriver = 0;
    unsigned int ID = pthread_self();
    sem_wait(&critSec);
    printf("Thread ID: %u, Team: A, I am looking for a car\n",ID);
    if (sleepingA >= 3)
    {
        pthread_barrier_init(&driverBar,NULL,4);
        sem_post(&waitingA);
        sem_post(&waitingA);
        sem_post(&waitingA);
        IsDriver = 1;
        sleepingA -=3;
    }else if (sleepingA >= 1 && sleepingB >= 2)
    {
        pthread_barrier_init(&driverBar,NULL,4);
        sem_post(&waitingA);
        sem_post(&waitingB);
        sem_post(&waitingB);
        IsDriver = 1;
        sleepingB -=2;
        sleepingA -=1;
    }else{
        sleepingA++;
        sem_post(&critSec);
        sem_wait(&waitingA);
    }

    printf("Thread ID: %u, Team: A, I have found a spot in a car\n",ID);
    pthread_barrier_wait(&driverBar);    
    
    if (IsDriver)
    {
        printf("Thread ID: %u, Team: A, I am the captain and driving the car\n",ID);
        sem_post(&critSec);
    }
    

}

void* SupporterB(void * args){
    int IsDriver = 0;
    unsigned int ID = pthread_self();
    sem_wait(&critSec);
    printf("Thread ID: %u, Team: B, I am looking for a car\n",ID);
    if (sleepingB >= 3)
    {
        pthread_barrier_init(&driverBar,NULL,4);
        sem_post(&waitingB);
        sem_post(&waitingB);
        sem_post(&waitingB);
        IsDriver = 1;
        sleepingB -=3;
    }else if (sleepingB >= 1 && sleepingA >= 2)
    {
        pthread_barrier_init(&driverBar,NULL,4);
        sem_post(&waitingB);
        sem_post(&waitingA);
        sem_post(&waitingA);
        IsDriver = 1;
        sleepingA -=2;
        sleepingB -=1;
    }else{
        sleepingB++;
        sem_post(&critSec);
        sem_wait(&waitingB);
    }

    printf("Thread ID: %u, Team: B, I have found a spot in a car\n",ID);
    pthread_barrier_wait(&driverBar);    
    
    if (IsDriver)
    {
        printf("Thread ID: %u, Team: B, I am the captain and driving the car\n",ID);
        sem_post(&critSec);
    }
    
}


int main(int argc, char*argv[]){
    
    int numA, numB;
    if (argc >= 3)
    {
        numA = atoi(argv[1]);
        numB = atoi(argv[2]);
    }else{
        printf("The main terminates\n");
        return 0;
    }
    
    if (numA%2 || numB%2 || (numA + numB)%4)
    {
        printf("The main terminates\n");
        return 0;
    }
    

    
    pthread_t threadA[numA];
    pthread_t threadB[numB];
    sem_init(&critSec,0,1);
    sem_init(&waitingA,0,0);
    sem_init(&waitingB,0,0);
    for (int i = 0; i < numA; i++)
    {
        pthread_create(&threadA[i],NULL,&SupporterA, NULL);
    }
    for (int i = 0; i < numB; i++)
    {
        pthread_create(&threadB[i],NULL,&SupporterB, NULL);
    }
    
    for (int i = 0; i < numA; i++)
    {
        pthread_join(threadA[i],NULL);
    }
    for (int i = 0; i < numB; i++)
    {
        pthread_join(threadB[i],NULL);
    }
    
    sem_destroy(&critSec);
    sem_destroy(&waitingA);
    sem_destroy(&waitingB);
    //pthread_barrier_destroy(&driverBar);

    printf("The main terminates\n");
    return 0;
}