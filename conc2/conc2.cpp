#include<iostream>
#include<pthread.h>
#include<stdoio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<time.h>



using namespace std;

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

void all_they_do_is_eat_and_think(){
	int x = 1;
     while(x==1){
	
     }	
}


int main(){
  srand(time(NULL));
   
   pthread_t Locke, Socrates, Plato, Confucius, Descartes;

  return 0;
}
