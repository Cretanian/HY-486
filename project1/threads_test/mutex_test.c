#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>



int shared_variable = 0;
pthread_mutex_t lock; 

void *
thread_function (void *myargs)
{
       pthread_mutex_lock(&lock); //try removing those locks
		shared_variable++;
	   pthread_mutex_unlock(&lock); 


}


int main () // ignore arguments
{
   
	int i;
    // create a barrier object with a count of 3
	pthread_t my_threads[1000];


	pthread_mutex_init(&lock, NULL);
	
	for(i = 0; i<1000; i++) {

		pthread_create (&(my_threads[i]), NULL, thread_function, NULL);

		
	}
  
	for(i = 0; i<1000; i++)   
		pthread_join((my_threads[i]), NULL);
	
	printf("Shared Variable: %d \n", shared_variable);

    // after this point, all three threads have completed.
}