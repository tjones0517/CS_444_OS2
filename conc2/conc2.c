#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<time.h>
#include<pthread.h>

sem_t forks[5];

int left(i){ 
   return i;
}

int right(i){ 
   return(i+1)%5;
}

void get_forks_lefty(int i){
   sem_wait(&forks[left(i)]);
   sem_wait(&forks[right(i)]);
}

void get_forks_righty(int i){
   sem_wait(&forks[right(i)]);
   sem_wait(&forks[left(i)]);
}

void put_forks(int i){
   sem_post(&forks[right(i)]);
   sem_post(&forks[left(i)]);
}

void think(int who){
	int thinking = 	rand() % 20 + 1;
	printf("%d: Thinking for %d seconds...\n", who, thinking);
	sleep(3);
}

void eat(int who){
	int eating = rand() % 9 + 2;
	printf("%d: Eating for %d seconds...\n", who, eating);
	sleep(eating);
}

void *all_they_do_is_eat_and_think(void *position){
     int x = 1;
     while(x==1){
	think(position);
        if(position==0){
	   get_forks_lefty(position);
           printf("lefty position\n");
	}	   
	else
	   get_forks_righty(position);
	eat(position);
	put_forks(position);
     }	
}


int main(){
   
   srand(time(NULL));

   sem_t f0,f1,f2,f3,f4;

   sem_init(&f0,0,1);
   sem_init(&f1,0,1);
   sem_init(&f2,0,1);
   sem_init(&f3,0,1);
   sem_init(&f4,0,1);

   forks[0] = f0;
   forks[1] = f1;
   forks[2] = f2;
   forks[3] = f3;
   forks[4] = f4;
   
   pthread_t Locke, Socrates, Plato, Confucius, Descartes;
   pthread_create(&Locke, NULL, all_they_do_is_eat_and_think, (void*)0);

   pthread_create(&Socrates, NULL, all_they_do_is_eat_and_think,(void*)1);
   pthread_create(&Plato, NULL, all_they_do_is_eat_and_think, (void*)2);
   pthread_create(&Confucius, NULL, all_they_do_is_eat_and_think, (void*)3);
   pthread_create(&Descartes, NULL, all_they_do_is_eat_and_think, (void*)4);

   pthread_join(&Locke, NULL);

   sleep(100000000);
  return 0;
}
