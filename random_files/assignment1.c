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

unsigned int eax;
unsigned int ebx;
unsigned int ecx;
unsigned int edx;

struct buffer_item buffer[b_size]; //an array of buffer_items of size 32

int generate_rand(int min, int max){
	unsigned long random;
	random = 0.0;
   	eax = 0x01;

	__asm__ __volatile__(
      			"cpuid;"
			: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
			: "a"(eax)
      			);

	if(ecx & 0x40000000){
	   	printf("using rdrand");
		unsigned int tmp=0;
		__asm__ __volatile__(
		      		     "rdrand %0;"
				     : "=r" (tmp)
		      		     );
		random = tmp % (max-min+1) +min;
		
	}
	else{
   
   		 
		random = genrand_int32();
		random = random % (max - min + 1) + min;
		//printf("random1: %d\n", random);
		//random = random * 10.0;
		//printf("random2: %d\n", random);
		//if(random>max)
	   		//random = max;
		//else if (random<min)
	   		//random = min;
		//printf("random is: %d\n", random);
		

	}
	return random;
}

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

void *consumer_c(void *tid){
	int sem_num;
	int thread_num = (int)tid;
	struct buffer_item event;

	while(x==1){
   		sem_wait(&items);
		pthread_mutex_lock(&mutex);
		
		sem_getvalue(&items, &sem_num);
		event = buffer[sem_num];
		
		pthread_mutex_unlock(&mutex);
		sem_post(&spaces);
		
		printf("CONSUMER %d: Sleep for %d seconds\n", thread_num+1, event.wait_time);
		sleep(event.wait_time);

		printf("CONSUMER %d: Value -> %d\n", thread_num+1, event.value);
    	}
}


int main(int argc, char **argv){ //initialize all semaphores to 0*/
   int num_p, num_c;

  // pthread_t threads[atoi(argv[1])];
   int i, j;

   num_p=generate_rand(2,7);
   num_c=generate_rand(2,7); 

   pthread_mutex_init(&mutex, NULL);
   sem_init(&items, 0, 0);
   sem_init(&spaces, 0, 32);

   for(i=0; i<atoi(argv[1]); i++){
      pthread_create(&thread_id, &attr, producer_c, (void*)i);
   }

   for(j=0; j<atoi(argv[2]); j++){
      pthread_create(&thread_id, &attr, consumer_c, (void*)j);
   }
   pthread_join(thread_id, NULL);
   
   //pthread_create(&thread_id, &attr, producer_c, (void*)1);
   //pthread_create(&thread_id, &attr, producer_c, (void*)2);
   
   //pthread_create(&thread_id, &attr, consumer_c, (void*)1);
   
   //pthread_create(&thread_id, &attr, consumer_c, (void*)2);
   
   //pthread_create(&thread_id, &attr, consumer_c, (void*)3);
      
   //pthread_join(thread_id, NULL);
   //pthread_join(thread_id2, NULL);
   //pthread_join(thread_id3, NULL);
   //pthread_join(thread_id4, NULL);
   //sleep(100000);
   
   return 0;

}
