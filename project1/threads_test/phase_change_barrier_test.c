#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct args{int id;} ;
pthread_barrier_t   phase_barrier; // the barrier synchronization object

void *
thread_function (void *myargs)
{
   printf (" Phase 1 Hello World ID: %d \n", ((struct  args *)myargs)->id);
      
	pthread_barrier_wait(&phase_barrier); //try removing this barrier

   if(((struct  args *)myargs)->id == 0)
	   printf ("\n -----------PHASE CHANGE-------------------------------- \n\n");
    pthread_barrier_wait(&phase_barrier); //try removing this barrier

   printf (" Phase 2 Hello World ID: %d \n", ((struct  args *)myargs)->id);

}


int main () // ignore arguments
{
   
	int i;
    // create a barrier object with a count of 3
	pthread_t my_threads[3];
    pthread_barrier_init (&phase_barrier, NULL, 3);
	
	for(i = 0; i<3; i++) {
		struct args * myargs = malloc(sizeof(struct args));
		myargs->id = i;
		pthread_create (&(my_threads[i]), NULL, thread_function, myargs);

		
	}
  
	for(i = 0; i<3; i++)   
		pthread_join((my_threads[i]), NULL);

	pthread_barrier_destroy(&phase_barrier);
	
    // after this point, all three threads have completed.
}