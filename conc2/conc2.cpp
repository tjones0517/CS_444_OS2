#include<iostream>
#include<pthread.h>
#include<stdoio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<time.h>



using namespace std;

srand(time(NULL));

sem_t f1,f2,f3,f4,f5;

sem_init(&f1,0,1);
sem_init(&f2,0,1);
sem_init(&f3,0,1);
sem_init(&f4,0,1);
sem_init(&f5,0,1);

sem_t forks [] = {f1, f2, f3, f4, f5};

int left(i){ 
   return i;
}

int right(i){ 
   return(i+1)%5;
}

void get_forks_lefty(i){
	forks[left(i)].wait();
	forks[right(i)].wait();
}

void get_forks_righty(i){
   	forks[right(i)].wait();
	forks[left(i)].wait();
}

void put_forks(i){
   forks[right(i)].signal();
   forks[left(i)].signal();
}

void think(){
	int thinking = 	rand() % 20 + 1;
	sprintf("Thinking for %d seconds...", &thinking);
	sleep(thinking);
}

void eat(){
	int eating = rand() % 9 + 2;
	sprintf("Eating for %d seconds...", &eating);
	sleep(eating);
}

void all_they_do_is_eat_and_think(int which_hand, int position){
     int x = 1;
     while(x==1){
	think();
        if(which_hand==0)
	   get_forks_lefty(position);	   
	else
	   get_forks_righty(position);
	eat();
	put_forks();
     }	
}


int main(){
   
   pthread_t Locke, Socrates, Plato, Confucius, Descartes;

  return 0;
}
