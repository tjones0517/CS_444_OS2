#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>

//Semaphore mutex, block;
sem_t mutex, block;
//pthread_mutex_t mutex, block;
pthread_t pt1, pt2, pt3, pt4, pt5, pt6, pt7, pt8;

int active = 0;
int waiting = 0;
bool wait = false;

int check_if_3(void *number){

int n;
int thread_num =  (int)number;
sem_wait(&mutex);

printf("thread %d\n", thread_num);

if(wait){
   waiting += 1; 
   sem_post(&mutex);
   sem_wait(&block);
   sem_wait(&mutex);
   waiting -= 1;
}

active += 1;

if(active ==3){
   wait = true;
}

sem_post(&mutex);

sleep(10);

sem_wait(&mutex);
active -=1;
if(!active){
   if (waiting < 3){
      n = waiting;
   }
   else{
      n = 3;
   }
   while(n > 0){
      sem_post(&block);
      n -=1;
   }
   wait = false;
}
sem_post(&mutex);

}

int main(void){
 //  sem_t mutex, block;
   int active = 0;
   int waiting = 0;
   bool wait = false;
   int i;

   sem_init(&mutex, 0,1);
   sem_init(&block, 0, 0);

   pthread_t pt1, pt2, pt3, pt4, pt5, pt6, pt7, pt8;
   pthread_create(&pt1, NULL, check_if_3, (void*)1);
   pthread_create(&pt2, NULL, check_if_3, (void*)2);
   pthread_create(&pt3, NULL, check_if_3, (void*)3);
   pthread_create(&pt4, NULL, check_if_3, (void*)4);
/*   pthread_create(&pt5, NULL, check_if_3, (void*)5);
   pthread_create(&pt6, NULL, check_if_3, (void*)6);
   pthread_create(&pt7, NULL, check_if_3, (void*)7);
   pthread_create(&pt8, NULL, check_if_3, (void*)8);
*/
   pthread_join(pt1, NULL);
   pthread_join(pt2, NULL);
   pthread_join(pt3, NULL);
   pthread_join(pt4, NULL);
  /* pthread_join(pt5, NULL);
   pthread_join(pt6, NULL);
   pthread_join(pt7, NULL);
   pthread_join(pt8, NULL);
   */
return 0;


}

