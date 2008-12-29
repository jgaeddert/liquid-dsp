//
// Generic port
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "buffer.h"

struct gport_s {
    void * v;
    unsigned int n;     // buffer size (elements)
    unsigned int N;     // num elements allocated
    unsigned int size;  // sizeof(element)

    // producer
    unsigned int write_index;
    //bool producer_lock;
    pthread_mutex_t producer_mutex;
    unsigned int num_write_elements_locked;
    unsigned int num_write_elements_available;
    unsigned int num_write_elements_requested;
    pthread_mutex_t producer_waiting;

    // consumer
    unsigned int read_index;
    //bool consumer_lock;
    pthread_mutex_t consumer_mutex;
    unsigned int num_read_elements_locked;
    unsigned int num_read_elements_available;
    unsigned int num_read_elements_requested;
    pthread_mutex_t consumer_waiting;

    // other
    pthread_mutex_t internal_mutex;
};

gport gport_create(unsigned int _n, unsigned int _sizeof)
{
    gport p = (gport) malloc(sizeof(struct gport_s));
    p->v = NULL;

    p->n = _n;
    p->N = 2*(p->n)-1;
    p->size = _sizeof;
    p->v = (void*) malloc((p->N)*(p->size));

    // producer
    pthread_mutex_init(&(p->producer_mutex),NULL);
    pthread_mutex_init(&(p->producer_waiting),NULL);
    p->write_index = 0;
    p->num_write_elements_available = p->n;
    p->num_write_elements_requested = 0;

    // consumer
    pthread_mutex_init(&(p->consumer_mutex),NULL);
    pthread_mutex_init(&(p->consumer_waiting),NULL);
    p->read_index = 0;
    p->num_read_elements_available = 0;
    p->num_read_elements_requested = 0;

    // internal
    pthread_mutex_init(&(p->internal_mutex),NULL);

    return p;
}

void gport_destroy(gport _p)
{
    // producer
    pthread_mutex_destroy(&(_p->producer_mutex));
    pthread_mutex_destroy(&(_p->producer_waiting));

    // consumer
    pthread_mutex_destroy(&(_p->consumer_mutex));
    pthread_mutex_destroy(&(_p->consumer_waiting));

    // other
    pthread_mutex_destroy(&(_p->internal_mutex));
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

void * gport_producer_lock(gport _p, unsigned int _n)
{
    // lock main producer mutex: only one producer at a time
    //printf("gport: producer waiting for lock...\n");
    pthread_mutex_lock(&(_p->producer_mutex));
    //printf("gport: producer locked\n");

    // TODO wait for _n elements to become available
    while (_n > _p->num_write_elements_available) {
        printf("warning/todo: gport_producer_lock(), wait for _n elements to become available\n");
        usleep(100000);
    }

    return _p->v + (_p->write_index)*(_p->size);
}

void gport_producer_unlock(gport _p, unsigned int _n)
{
    pthread_mutex_lock(&(_p->internal_mutex));

    // TODO validate number added

    _p->num_write_elements_available -= _n;
    _p->num_read_elements_available += _n;

    // copy overflow from residual memory
    if (_p->write_index + _n > _p->n) {
        printf("gport: producer copying overflow (%u elements)\n", 0);

        memmove(_p->v,
                _p->v + (_p->n)*(_p->size),
                ((_p->write_index + _n)-(_p->n))*(_p->size));
    }

    _p->write_index = (_p->write_index + _n) % _p->n;

    pthread_mutex_unlock(&(_p->internal_mutex));

    //printf("gport: producer unlocked\n");
    pthread_mutex_unlock(&(_p->producer_mutex));
}


// consumer methods

void * gport_consumer_lock(gport _p, unsigned int _n)
{
    // lock main consumer mutex: only one consumer at a time
    //printf("gport: consumer waiting for lock...\n");
    pthread_mutex_lock(&(_p->consumer_mutex));
    //printf("gport: consumer locked\n");

    while (_n > _p->num_read_elements_available) {
        printf("warning/todo: gport_consumer_lock(), wait for _n elements to become available\n");
        usleep(100000);
    }

    pthread_mutex_lock(&(_p->internal_mutex));

    // copy underflow to residual memory
    if (_n > (_p->n - _p->read_index)) {
        printf("gport: consumer copying underflow\n");

        memmove(_p->v + (_p->n)*(_p->size),
                _p->v,
                (_n - (_p->n - _p->read_index))*(_p->size));
    }
    pthread_mutex_unlock(&(_p->internal_mutex));

    return _p->v + (_p->read_index)*(_p->size);
}

void gport_consumer_unlock(gport _p, unsigned int _n)
{
    // TODO validate number released

    pthread_mutex_lock(&(_p->internal_mutex));

    _p->num_read_elements_available -= _n;
    _p->num_write_elements_available += _n;
    _p->read_index = (_p->read_index + _n) % _p->n;

    pthread_mutex_unlock(&(_p->internal_mutex));

    //printf("gport: consumer unlocked\n");
    pthread_mutex_unlock(&(_p->consumer_mutex));
}

