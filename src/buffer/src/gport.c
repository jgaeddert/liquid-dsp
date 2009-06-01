/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Generic port
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "liquid.internal.h"

struct gport_s {
    void * v;
    unsigned int n;     // buffer size (elements)
    unsigned int N;     // num elements allocated
    size_t size;        // sizeof(element)

    // producer
    unsigned int write_index;
    pthread_mutex_t producer_mutex;
    unsigned int num_write_elements_available;
    pthread_cond_t producer_data_ready;
    bool producer_waiting;

    // consumer
    unsigned int read_index;
    pthread_mutex_t consumer_mutex;
    unsigned int num_read_elements_available;
    pthread_cond_t consumer_data_ready;
    bool consumer_waiting;

    // other
    pthread_mutex_t internal_mutex;
};

gport gport_create(unsigned int _n, unsigned int _size)
{
    gport p = (gport) malloc(sizeof(struct gport_s));
    p->v = NULL;

    p->n = _n;
    p->N = 2*(p->n)-1;
    p->size = (size_t)_size;
    p->v = (void*) malloc((p->N)*(p->size));

    // producer
    pthread_mutex_init(&p->producer_mutex,NULL);
    p->write_index = 0;
    p->num_write_elements_available = p->n;
    pthread_cond_init(&p->producer_data_ready,NULL);

    // consumer
    pthread_mutex_init(&p->consumer_mutex,NULL);
    p->read_index = 0;
    p->num_read_elements_available = 0;
    pthread_cond_init(&p->consumer_data_ready,NULL);

    // internal
    pthread_mutex_init(&p->internal_mutex,NULL);

    return p;
}

void gport_destroy(gport _p)
{
    // producer
    pthread_mutex_destroy(&_p->producer_mutex);
    pthread_cond_destroy(&_p->producer_data_ready);

    // consumer
    pthread_mutex_destroy(&_p->consumer_mutex);
    pthread_cond_destroy(&_p->consumer_data_ready);

    // other
    pthread_mutex_destroy(&_p->internal_mutex);
    free(_p->v);
    free(_p);
}

void gport_print(gport _p)
{
    pthread_mutex_lock(&_p->internal_mutex);

    printf("gport: [%u @ %u bytes]\n", _p->n, (unsigned int)(_p->size));
    unsigned int i,j,n=0;
    unsigned char * s = (unsigned char*) (_p->v);
    for (i=0; i<_p->n; i++) {
        printf("  %3u:  0x", i);
        for (j=0; j<_p->size; j++) {
            printf("%.2x", s[n++]);
        }
        printf("\n");
    }

    pthread_mutex_unlock(&_p->internal_mutex);
}

// producer methods

void * gport_producer_lock(gport _p, unsigned int _n)
{
    // lock main producer mutex: only one producer at a time
    //printf("gport: producer waiting for lock...\n");
    pthread_mutex_lock(&_p->producer_mutex);
    //printf("gport: producer locked\n");

    pthread_mutex_lock(&_p->internal_mutex);

    // TODO wait for _n elements to become available
    while (_n > _p->num_write_elements_available) {
        //printf("warning/todo: gport_producer_lock(), wait for _n elements to become available\n");
        //usleep(100000);
        _p->producer_waiting = true;
        pthread_cond_wait(&(_p->producer_data_ready),&(_p->internal_mutex));
        //printf("gport: producer received signal: data ready\n");
    }
    _p->producer_waiting = false;

    pthread_mutex_unlock(&_p->internal_mutex);

    return _p->v + (_p->write_index)*(_p->size);
}

void gport_producer_unlock(gport _p, unsigned int _n)
{
    pthread_mutex_lock(&_p->internal_mutex);

    // TODO validate number added

    _p->num_write_elements_available -= _n;
    _p->num_read_elements_available += _n;

    // copy overflow from residual memory
    if (_p->write_index + _n > _p->n) {
        //printf("gport: producer copying overflow (%u elements)\n", 0);

        memmove(_p->v,
                _p->v + (_p->n)*(_p->size),
                ((_p->write_index + _n)-(_p->n))*(_p->size));
    }

    _p->write_index = (_p->write_index + _n) % _p->n;

    // if consumer is waiting for data, signal its availability
    if (_p->consumer_waiting) {
        //printf("gport: producer sending signal: data ready\n");
        pthread_cond_signal(&(_p->consumer_data_ready));
    }

    pthread_mutex_unlock(&_p->internal_mutex);

    //printf("gport: producer unlocked\n");
    pthread_mutex_unlock(&_p->producer_mutex);
}


// consumer methods

void * gport_consumer_lock(gport _p, unsigned int _n)
{
    // lock main consumer mutex: only one consumer at a time
    //printf("gport: consumer waiting for lock...\n");
    pthread_mutex_lock(&_p->consumer_mutex);
    //printf("gport: consumer locked\n");

    pthread_mutex_lock(&_p->internal_mutex);

    while (_n > _p->num_read_elements_available) {
        //printf("warning/todo: gport_consumer_lock(), wait for _n elements to become available\n");
        //usleep(100000);
        _p->consumer_waiting = true;
        pthread_cond_wait(&(_p->consumer_data_ready),&(_p->internal_mutex));
        //printf("gport: consumer received signal: data ready\n");
    }
    _p->consumer_waiting = false;

    // copy underflow to residual memory
    if (_n > (_p->n - _p->read_index)) {
        //printf("gport: consumer copying underflow\n");

        memmove(_p->v + (_p->n)*(_p->size),
                _p->v,
                (_n - (_p->n - _p->read_index))*(_p->size));
    }
    pthread_mutex_unlock(&_p->internal_mutex);

    return _p->v + (_p->read_index)*(_p->size);
}

void gport_consumer_unlock(gport _p, unsigned int _n)
{
    // TODO validate number released

    pthread_mutex_lock(&_p->internal_mutex);

    _p->num_read_elements_available -= _n;
    _p->num_write_elements_available += _n;
    _p->read_index = (_p->read_index + _n) % _p->n;

    // if producer is waiting for data, signal its availability
    if (_p->producer_waiting) {
        //printf("gport: consumer sending signal: data ready\n");
        pthread_cond_signal(&(_p->producer_data_ready));
    }

    pthread_mutex_unlock(&_p->internal_mutex);

    //printf("gport: consumer unlocked\n");
    pthread_mutex_unlock(&_p->consumer_mutex);
}

