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
int value;

int check_if_3(void *number){

	while(1){
		int n;
		int thread_num = (int)number;

		sem_wait(&mutex);
		sem_getvalue(&mutex, &value);
	//	printf("thread %d\n", thread_num);
		
		if(wait){
		   waiting += 1;
		   sleep(2); 
		   printf("thread %d is waiting \n", thread_num);
		   sem_post(&mutex);
		   sem_wait(&block);
		}

		else {
		   active +=1;
		 // printf("active in else: %d\n", active);
	//	   printf("thread %d is ACTIVE\n", thread_num);
		   if(active == 3){
			  wait = true;
		   }
		   else {
		      wait = false;
		   }
		   sem_post(&mutex);
		  // waiting -= 1;
		}

		//active += 1;

//	printf("thread %d is ACTIVE\n", thread_num);
		/*
		if(active ==3){
		   wait = true;
		}
		*/
		sleep(thread num +2);

	//	printf("thread %d is going to sleep\n", thread_num);
		sem_wait(&mutex);
		 printf("NUM OF ACTIVE  %d\n", active);
		active -=1;
	       	printf("thread %d is leaving\n", thread_num);


		//printf("thread %d is going to sleep\n", thread_num);
	//	sleep(3);

	//	   printf("active after: %d\n", active);
		if (active ==0){
		//printf("thread %d is awake\n", thread_num);
		//sem_wait(&mutex);
		//active -=1;

		   if (waiting < 3){
			  n = waiting;
		   }
		   else{
			  n = 3;
		   }
		   waiting -= n;
		   active = n;

		   while(n > 0){
			  sem_post(&block);
			  n -=1;
			 // printf("thread is leaving\n", thread_num);
		   }
		   if (active == 3){
		   wait = true;
		   }
		   else {
		      wait = false;
		   }
		}

	//	printf("thread %d is LEAVING\n", thread_num);
		sem_post(&mutex);
	}
}

int main(void){
 //  sem_t mutex, block;
 /*
   int active = 0;
   int waiting = 0;
   bool wait = false;
*/
   int i;

   sem_init(&mutex, 0,1);
   sem_init(&block, 0, 4);

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
