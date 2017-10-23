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
   printf("Fork %d: PICKED UP\n", left(i));
   sem_wait(&forks[right(i)]);
   printf("Fork %d: PICKED UP\n", right(i));
}

void get_forks_righty(int i){
   sem_wait(&forks[right(i)]);
   printf("Fork %d: PICKED UP\n", right(i));
   sem_wait(&forks[left(i)]);
   printf("Fork %d: PICKED UP\n", left(i));
}

void put_forks(int i){
   sem_post(&forks[right(i)]);
   printf("Fork %d: READY FOR USE\n", right(i));
   sem_post(&forks[left(i)]);
   printf("Fork %d: READY FOR USE\n", left(i));
}

void think(char* name){
	int thinking = 	rand() % 20 + 1;
	printf("%s: Thinking for %d seconds...\n", name, thinking);
	sleep(thinking);
}

void eat(char* name){
	int eating = rand() % 9 + 2;
	printf("%s: Eating for %d seconds...\n", name, eating);
	sleep(eating);
}

void *all_they_do_is_eat_and_think(void *position){
     int x = 1;
     char* name;
     if(position==0){
	name = "Locke";
     }
     if(position==1){
        name = "Socrates";
     }
     if(position==2){
        name = "Plato";
     }
     if(position==3){
        name = "Confucius";
     }
     if(position==4){
        name = "Descartes";
     }	
     while(x==1){
	think(name);
        if(position==0){
	   get_forks_lefty(position);
	}	   
	else
	   get_forks_righty(position);
	eat(name);
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

   pthread_join(Locke, NULL);
   pthread_join(Socrates, NULL);
   pthread_join(Plato, NULL);
   pthread_join(Confucius, NULL);
   pthread_join(Descartes, NULL);

  
  return 0;
}
