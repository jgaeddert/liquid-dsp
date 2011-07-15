//
// gport_dma_threaded_example.c
// 
// generic port (gport) direct memory access example (threaded)
//

#include <unistd.h>     // Symbolic Constants
#include <sys/types.h>  // Primitive System Data Types
#include <errno.h>      // Errors
#include <stdio.h>      // Input/Output
#include <stdlib.h>     // General Utilities
#include <pthread.h>    // POSIX Threads
#include <string.h>     // String handling

#include "liquid.experimental.h"

// prototype for thread routines
void producer_handler ( void *ptr );
void consumer_handler ( void *ptr );

#define PRODUCER_TIMER      100     // sleep time (ms) after producer writes samples
#define NUM_SAMPLES_TOTAL   20      // total number of samples to flow through port
#define PRODUCER_SIZE_MAX   3       // maximum producer size
#define CONSUMER_SIZE_MAX   5       // maximum consumer size

int main()
{
    pthread_t producer_thread;
    pthread_t consumer_thread;
    pthread_attr_t thread_attr;
    void * status;
    
    // create port: interface between threads
    gport p = gport_create(PRODUCER_SIZE_MAX + CONSUMER_SIZE_MAX, sizeof(unsigned int));
    
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
    unsigned int i, n=0;
    int * w;
    unsigned int num_samples_remaining = NUM_SAMPLES_TOTAL;

    while (num_samples_remaining > 0) {
        // randomly choose producer size
        unsigned int k = (rand() % PRODUCER_SIZE_MAX) + 1;
        if (k > num_samples_remaining)
            k = num_samples_remaining;

        printf("  producer waiting for %u samples...\n", k);
        w = (int*) gport_producer_lock(p,k);

        printf("  producer writing %u samples...\n", k);
        for (i=0; i<k; i++)
            w[i] = n++;

        printf("  producer waiting %u ms\n", PRODUCER_TIMER);
        usleep(PRODUCER_TIMER*1000);

        printf("  producer unlocking port\n");
        gport_producer_unlock(p,k);

        num_samples_remaining -= k;
    }
    
    printf("  producer exiting thread\n");
    pthread_exit(0); // exit thread
}


void consumer_handler ( void *_ptr )
{
    gport p = (gport) _ptr;
    unsigned int i, n=0;
    int * r;
    unsigned int num_samples_remaining = NUM_SAMPLES_TOTAL;

    while (num_samples_remaining > 0) {
        // randomly choose consumer size
        unsigned int k = (rand() % CONSUMER_SIZE_MAX) + 1;
        if (k > num_samples_remaining)
            k = num_samples_remaining;

        printf("  consumer waiting for %u samples...\n", k);
        r = (int*) gport_consumer_lock(p,k);

        for (i=0; i<k; i++)
            printf("  %3u: %d\n", n++, r[i]);

        printf("  consumer unlocking port\n");
        gport_consumer_unlock(p, k);

        num_samples_remaining -= k;
    }

    printf("  consumer exiting thread\n");
    pthread_exit(0); // exit thread
}


