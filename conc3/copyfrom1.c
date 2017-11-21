#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include "mt19937ar.h"
#define b_size 32

pthread_mutex_t mutex;
pthread_attr_t attr;
pthread_t thread_id, thread_id2, thread_id3, thread_id4, thread_id5;
sem_t items, spaces;
int x = 1;

struct buffer_item {
   int value;
   int wait_time;

};

struct args{
   	long tid;
	long sleep_time;
};


struct buffer_item buffer[b_size]; //an array of buffer_items of size 32

void *producer_c(void *tid){
	int sem_num, val, rwait, pwait;
	int thread_num = (int)tid;

   	while(x==1){
		
		val = generate_rand(1,9); //first value just a random number
		rwait = generate_rand(2,9); //value that consumer will sleep for before printing val

		pwait = generate_rand(3,7); //wait time before producing new item

	
		printf("PRODUCER %d: Sleep for %d seconds\n", thread_num+1, pwait);
   		sleep(pwait);
		
		struct buffer_item event= { val, rwait };

	
		sem_wait(&spaces);
		pthread_mutex_lock(&mutex);
		
		sem_getvalue(&items, &sem_num);
		buffer[sem_num]=event;
		
		pthread_mutex_unlock(&mutex);
		sem_post(&items);
		
		printf("PRODUCER %d: New event added!\n", thread_num+1);
		
   	}	
}



int main(int argc, char **argv){ //initialize all semaphores to 0*/
   int num_p, num_c;

   int i, j;

   num_p=generate_rand(2,7);
   num_c=generate_rand(2,7); 

   pthread_mutex_init(&mutex, NULL);
   sem_init(&items, 0, 0);
   sem_init(&spaces, 0, 3);

   pthread_create(&thread_id, &attr, producer_c, (void*)1);
   pthread_create(&thread_id, &attr, producer_c, (void*)2);
   pthread_create(&thread_id, &attr, producer_c, (void*)3);

   pthread_join(thread_id, NULL);
   
   return 0;

}
