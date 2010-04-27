//
// gport_ima_threaded_example.c
//
// gport (generic port) indirect memory access example (threaded)
//

#include <unistd.h>     // Symbolic Constants
#include <sys/types.h>  // Primitive System Data Types
#include <errno.h>      // Errors
#include <stdio.h>      // Input/Output
#include <stdlib.h>     // General Utilities
#include <pthread.h>    // POSIX Threads
#include <string.h>     // String handling

#include "liquid.h"

// prototype for thread routines
void producer_handler ( void *ptr );
void consumer_handler ( void *ptr );

#define PRODUCER_TIMER  100 // sleep (ms)
#define PRODUCER_SIZE   3
#define CONSUMER_SIZE   5

int main()
{
    pthread_t producer_thread;
    pthread_t consumer_thread;
    pthread_attr_t thread_attr;
    void * status;
    
    // create port: interface between threads
    gport p = gport_create(PRODUCER_SIZE+CONSUMER_SIZE, sizeof(unsigned int));
    
    // set thread attributes
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    // create threads
    pthread_create(&producer_thread, &thread_attr, (void*) &producer_handler, (void*) p);
    pthread_create(&consumer_thread, &thread_attr, (void*) &consumer_handler, (void*) p);

    // attributes object no longer needed
    pthread_attr_destroy(&thread_attr);

    // join threads
    pthread_join(producer_thread, &status);
    pthread_join(consumer_thread, &status);

    gport_destroy(p);

    printf("done.\n");
    return 0;
}

void producer_handler ( void *_ptr )
{
    gport p = (gport) _ptr;
    unsigned int i, j, n=0;
    int w[PRODUCER_SIZE];

    for (i=0; i<CONSUMER_SIZE; i++) {
        for (j=0; j<PRODUCER_SIZE; j++)
            w[j] = n++;

        printf("  producer waiting for %u samples...\n", PRODUCER_SIZE);
        gport_produce(p,(void*)w,PRODUCER_SIZE);

        printf("  producer waiting %u ms\n", PRODUCER_TIMER);
        usleep(PRODUCER_TIMER*1000);
    }
    
    printf("  producer exiting thread\n");
    pthread_exit(0); // exit thread
}


void consumer_handler ( void *_ptr )
{
    gport p = (gport) _ptr;
    unsigned int i, j, n=0;
    int r[CONSUMER_SIZE];

    for (i=0; i<PRODUCER_SIZE; i++) {
        printf("  consumer waiting for %u samples...\n", CONSUMER_SIZE);
        gport_consume(p,(void*)r,CONSUMER_SIZE);

        for (j=0; j<CONSUMER_SIZE; j++)
            printf("  %3u: %d\n", n++, r[j]);
    }

    printf("  consumer exiting thread\n");
    pthread_exit(0); // exit thread
}


