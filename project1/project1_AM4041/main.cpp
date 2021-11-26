#ifndef MAIN
#define MAIN

#include "structs.h"
#include "utils.h"
#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <vector>
#endif

using namespace std;

pthread_barrier_t   barrier_phase_end;
long N;
long M;
vector<Queue> Categories;
SinglyLinkedList manager;
treeNode* root;

void * thread_function (void *myargs)
{
    long expexted_size = 0, expexted_sum = 0, queue_num = 0;
   
    /*PASHE 1*/
    {
        pthread_barrier_wait(&barrier_phase_end);

            for(long i = 0; i < N; i++)
                assert(Insert((((LLNode *)myargs)->get_postID()) + i * M));
        
        pthread_barrier_wait(&barrier_phase_end);
       
        if(((LLNode *)myargs)->get_postID() == 0)
        {
            cout << "\n\n";
            expexted_size = ( 2 * pow(N,2));
            expexted_sum = (( 2 * pow(N,4)) -  pow(N,2));
            check_indexes_phase1(expexted_size, expexted_sum);
            cout << "\n -----------------PHASE 1 COMPLETE-----------------\n\n";
        }

        pthread_barrier_wait(&barrier_phase_end);
    }
    
    /*PASHE 2*/
    {    
        if  (-1 < (((LLNode *)myargs)->get_postID()) && 
             (((LLNode *)myargs)->get_postID()) < N)
        {
            queue_num =  (((LLNode *)myargs)->get_postID()) % (N/4);
            
            for(long i = 0; i < N; i++)
            {
                if(Search((((LLNode *)myargs)->get_postID()) + i * M))
                {
                     assert(Delete((((LLNode *)myargs)->get_postID()) + i * M));
                     assert(Enq(((((LLNode *)myargs)->get_postID()) + i * M), queue_num ));
                }
                if(Search((((LLNode *)myargs)->get_postID()) + i * M + N))
                {
                    assert(Delete((((LLNode *)myargs)->get_postID()) + i * M + N));
                    assert(Enq(((((LLNode *)myargs)->get_postID()) + i * M + N), queue_num));
                }
            }
        }

        pthread_barrier_wait(&barrier_phase_end);

        if(((LLNode *)myargs)->get_postID() == 0)
        {
            expexted_size = (8 * N);
            expexted_sum = (( 2 * pow(N, 4)) -  pow(N, 2));
            check_indexes_phase2(expexted_size, expexted_sum);
            check_indexes_phase1(0, -1);
            cout << "\n -----------------PHASE 2 COMPLETE-----------------\n\n";
        }

        pthread_barrier_wait(&barrier_phase_end);
    }
    
    /*PASHE 3*/
    {    
        if  (N-1 < (((LLNode *)myargs)->get_postID()) && 
             (((LLNode *)myargs)->get_postID()) < (2*N))
        {
            queue_num =  (((LLNode *)myargs)->get_postID()) % (N/4);
            long rewritten_postID = -1;
            for(long i = 0; i < N; i++)
            {
               rewritten_postID = Deq(queue_num);
               assert(rewritten_postID != -1);
               assert(BTSinsert(rewritten_postID));
            }
        }

        pthread_barrier_wait(&barrier_phase_end);

        if(((LLNode *)myargs)->get_postID() == N)
        {
            check_indexes_phase3(pow(N, 2));
            check_indexes_phase2((4*N), -1);
            cout << "\n -----------------PHASE 3 COMPLETE-----------------\n\n";
        }
            
        pthread_barrier_wait(&barrier_phase_end);
    }

    /*PASHE 4*/
    {    
        if  (-1 < (((LLNode *)myargs)->get_postID()) && 
             (((LLNode *)myargs)->get_postID()) < N)
        {
            queue_num =  (((LLNode *)myargs)->get_postID()) % (N/4);
            
            for(long i = 0; i < N; i++)
            {
                if(BTSsearch((((LLNode *)myargs)->get_postID()) + i * M))
                {
                     assert(BTSdelete((((LLNode *)myargs)->get_postID()) + i * M));
                     assert(Enq(((((LLNode *)myargs)->get_postID()) + i * M), queue_num ));
                }
                if(BTSsearch((((LLNode *)myargs)->get_postID()) + i * M + N))
                {
                    assert(BTSdelete((((LLNode *)myargs)->get_postID()) + i * M + N));
                    assert(Enq(((((LLNode *)myargs)->get_postID()) + i * M + N), queue_num));
                }
            }
        }

        pthread_barrier_wait(&barrier_phase_end);

        if(((LLNode *)myargs)->get_postID() == 0)
        {
            check_indexes_phase3(0);
            expexted_size = (8 * N);
            expexted_sum = (( 2 * pow(N, 4)) -  pow(N, 2));
            check_indexes_phase2(expexted_size, expexted_sum);
            cout << "\n -----------------PHASE 4 COMPLETE-----------------\n\n";
        }
            
        pthread_barrier_wait(&barrier_phase_end);
    }
}

void init(){
    LLNode* sentinelH  = new LLNode();
    LLNode* sentinelT = new LLNode();
    sentinelH->init_postID(-1);
    sentinelT->init_postID(-1);
    sentinelH->set_next(sentinelT);
    manager.set_head(sentinelH);
    manager.set_tail(sentinelT);

   QueueNode* sentinelQH;
    for(long i = 0; i < N/4; i++){
        Queue new_queue;
        sentinelQH = new QueueNode();
        sentinelQH->set_postID(-1);
        new_queue.set_head(sentinelQH);
        new_queue.set_tail(sentinelQH);
        Categories.push_back(new_queue);
    }

    root = new treeNode();
    root->set_left_child(root);
}

int main(int argc, char** argv) {
    
    N = atoi(argv[1]);

    if(N % 4 != 0){
        assert(false);
    }

    M = 2*N;

    init();

	pthread_t my_threads[M];

    pthread_barrier_init (&barrier_phase_end, NULL, M);

	for(long i = 0; i < M; i++)
    {
		LLNode *publisher = new LLNode();
        publisher->init_postID(i);
		pthread_create (&(my_threads[i]), NULL, thread_function, publisher);
	}
  
	for(long i = 0; i < M; i++)   
		pthread_join((my_threads[i]), NULL);

	pthread_barrier_destroy(&barrier_phase_end);
   
    return 0;
}