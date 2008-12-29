//
// Generic port
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "buffer.h"

struct gport_s {
    void * v;
    unsigned int n;     // buffer size
    unsigned int N;     // num elements allocated
    unsigned int size;  // sizeof(element)

    // producer
    unsigned int write_index;
    //bool producer_lock;
    pthread_mutex_t producer_mutex;
    unsigned int num_write_elements_locked;
    unsigned int num_write_elements_available;

    // consumer
    unsigned int read_index;
    //bool consumer_lock;
    pthread_mutex_t consumer_mutex;
    unsigned int num_read_elements_locked;
    unsigned int num_read_elements_available;
};

gport gport_create(unsigned int _n, unsigned int _sizeof)
{
    gport p = (gport) malloc(sizeof(struct gport_s));
    p->v = NULL;

    p->n = _n;
    p->N = 2*(p->n)-1;
    p->size = _sizeof;
    p->v = (void*) malloc((p->N)*(p->size));

    pthread_mutex_init(&(p->producer_mutex),NULL);
    pthread_mutex_init(&(p->consumer_mutex),NULL);

    return p;
}

void gport_destroy(gport _p)
{
    free(_p->v);
    free(_p);
}

void gport_print(gport _p)
{
    printf("gport: [%u @ %u]\n", _p->n, _p->size);
    unsigned int i,j,n=0;
    unsigned char * s = (unsigned char*) (_p->v);
    for (i=0; i<_p->n; i++) {
        printf("  %3u:  0x", i);
        for (j=0; j<_p->size; j++) {
            printf("%.2x", s[n++]);
        }
        printf("\n");
    }
}

// producer methods

void gport_producer_lock(gport _p, unsigned int _n, void ** _w)
{
    pthread_mutex_lock(&(_p->producer_mutex));
}

unsigned int gport_producer_get_num_locked(gport _p)
{
    return 0;
}

void gport_producer_add(gport _p, unsigned int _n)
{

}

void gport_producer_unlock(gport _p)
{
    pthread_mutex_unlock(&(_p->producer_mutex));
}


// consumer methods

void gport_consumer_lock(gport _p, unsigned int _n, void ** _r)
{

}

unsigned int gport_consumer_get_num_locked(gport _p)
{
    return 0;
}


void gport_consumer_release(gport _p, unsigned int _n)
{

}

void gport_consumer_unlock(gport _p)
{

}

