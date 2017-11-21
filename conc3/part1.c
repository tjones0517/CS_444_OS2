#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include "mt19937ar.h"
#define b_size 32

//Semaphore mutex, block;
sem_t mutex, block;

int active = 0;
int waiting = 0;
bool wait = false;

mutex.P();
if(wait){
   waiting += 1; 
   mutex.V();
   block.P();
   mutex.P();
   waiting -= 1;
}

active += 1;

if(active ==3){
   wait = true;
}

mutex.V();


mutex.P();
active -=1;
if(!active){
   int n(min(waiting,3));
   while(n > 0){
      block.V();
      n -=1;
   }
   wait = false;
}
mutex.V();

}

int main(){
   int i;
   sem_t mutex, block;

   sem_init(&mutex, 0, 3);
   sem_init(&block, 0, 3);

   for(i = 0 ; i < 6 ; i++ ){
   pthread_create(&thread_id, &attr, check_3, (void*)i);
   }

   pthread_join(thread_id,NULL);





}

