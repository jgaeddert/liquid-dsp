/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "liquid.experimental.h"

// gport internal structure
struct gport_s {
    void * v;           // internal memory buffer
    unsigned int n;     // buffer size (elements)
    unsigned int N;     // num elements allocated
    size_t size;        // sizeof(element)

    // producer
    unsigned int write_index;
    pthread_mutex_t producer_mutex;
    unsigned int num_write_elements_available;
    pthread_cond_t producer_data_ready;
    int producer_waiting;

    // consumer
    unsigned int read_index;
    pthread_mutex_t consumer_mutex;
    unsigned int num_read_elements_available;
    pthread_cond_t consumer_data_ready;
    int consumer_waiting;

    // other
    pthread_mutex_t internal_mutex;
    int eom;
};

// create gport object
//
//  _n      :   size of the internal buffer
//  _size   :   size of each element in bytes (e.g. sizeof(int))
gport gport_create(unsigned int _n, unsigned int _size)
{
    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: gport_create(), buffer length cannot be zero\n");
        exit(1);
    } else if (_size == 0) {
        fprintf(stderr,"error: gport_create(), object size cannot be zero\n");
        exit(1);
    }

    // allocate memory for port object
    gport p = (gport) malloc(sizeof(struct gport_s));

    // initialize internals
    p->n = _n;
    p->N = 2*(p->n)-1;
    p->size = (size_t)_size;
    p->v = (void*) malloc((p->N)*(p->size));

    // producer
    pthread_mutex_init(&p->producer_mutex,NULL);
    p->write_index = 0;
    p->num_write_elements_available = p->n;
    //p->num_write_elements_requested = 0;
    pthread_cond_init(&p->producer_data_ready,NULL);

    // consumer
    pthread_mutex_init(&p->consumer_mutex,NULL);
    p->read_index = 0;
    p->num_read_elements_available = 0;
    //p->num_read_elements_requested = 0;
    pthread_cond_init(&p->consumer_data_ready,NULL);

    // internal
    pthread_mutex_init(&p->internal_mutex,NULL);
    p->eom = 0;

    return p;
}

// destroy gport object and free all internal memory
void gport_destroy(gport _p)
{
    // broadcast eom signal
    gport_signal_eom(_p);

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

// print gport object internals
void gport_print(gport _p)
{
    pthread_mutex_lock(&_p->internal_mutex);

    printf("gport: [%u @ %u bytes]\n", _p->n, (unsigned int) (_p->size));
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

// 
// producer indirect memory access (IMA) methods
//

// produce data (indirect memory access)
//  _p      :   gport object
//  _w      :   external data buffer [size: 1 x _n]
//  _n      :   number of elements to produce (size of _w)
// returns status
int gport_produce(gport _p,
                  void * _w,
                  unsigned int _n)
{
    unsigned int num_produced;
    unsigned int num_produced_total = 0;

    // produce samples as they become available
    while (_n > 0 && !_p->eom) {
        // produce maximum number of samples available
        gport_produce_available(_p,
                                 _w + num_produced_total*_p->size,
                                 _n,
                                 &num_produced);
        // update external counters
        num_produced_total += num_produced;
        _n -= num_produced;
    }

    return _p->eom;
}

// produce available data (indirect memory access)
//  _p      :   gport object
//  _w      :   external data buffer [size: 1 x _nmax]
//  _nmax   :   number of elements in _w
//  _np     :   returned number of elements in _w produced by _p
// returns status
int gport_produce_available(gport _p,
                            void * _w,
                            unsigned int _nmax,
                            unsigned int *_np)
{
    // lock mutexes
    pthread_mutex_lock(&(_p->producer_mutex));
    pthread_mutex_lock(&(_p->internal_mutex));

    // wait until at least one element is available
    while (_p->num_write_elements_available == 0 && !_p->eom) {
        // set flag signaling that the producer is waiting for data
        _p->producer_waiting = 1;

        // Block on condition that space is availble in the buffer
        // for writing (producing). Invoking pthread_cond_wait()
        // unlocks 'internal_mutex,' allowing the consumer to read
        // values from the buffer.
        pthread_cond_wait(&(_p->producer_data_ready),
                          &(_p->internal_mutex));
    }

    // clear producer waiting flag
    _p->producer_waiting = 0;

    if (_p->eom) {
        pthread_mutex_unlock(&(_p->internal_mutex));
        pthread_mutex_unlock(&(_p->producer_mutex));
        *_np = 0;
        return _p->eom;
    }

    // set return value (number of samples we have produced)
    unsigned int n = (_p->num_write_elements_available > _nmax) ?
        _nmax : _p->num_write_elements_available;
    *_np = n;

    // copy data circularly if necessary
    if (_p->write_index + n > _p->n) {
        // overflow: copy data circularly
        unsigned int b = _p->n - _p->write_index;

        // copy lower section: 'b' elements
        memmove(_p->v + (_p->write_index)*(_p->size),
                _w,
                b*(_p->size));
        
        // copy upper section
        memmove(_p->v,
                _w + b*(_p->size),
                (n-b)*(_p->size));
    } else {
        memmove(_p->v + (_p->write_index)*(_p->size),
                _w,
                n*(_p->size));
    }

    // update internal counters
    _p->num_write_elements_available -= n;
    _p->num_read_elements_available  += n;

    // update internal write index
    _p->write_index = (_p->write_index + n) % _p->n;

    // if consumer is waiting, signal condition
    if (_p->consumer_waiting)
        pthread_cond_signal(&(_p->consumer_data_ready));

    // release mutexes
    pthread_mutex_unlock(&(_p->internal_mutex));
    pthread_mutex_unlock(&(_p->producer_mutex));
    return _p->eom;
}

// 
// producer direct memory access (DMA) methods
//

// lock producer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to lock
//
// returns pointer to internal memory buffer for writing
void * gport_producer_lock(gport _p,
                           unsigned int _n)
{
    // validate input
    if (_n > _p->n) {
        fprintf(stderr,"error: gport_producer_lock(), lock requested for %u samples, exceeding buffer size\n", _n);
        exit(1);
    }

    // lock main producer mutex: only one producer at a time
    //printf("gport: producer waiting for lock...\n");
    pthread_mutex_lock(&_p->producer_mutex);
    //printf("gport: producer locked\n");

    pthread_mutex_lock(&_p->internal_mutex);

    // TODO wait for _n elements to become available
    // TODO exit on eom flag
    while (_n > _p->num_write_elements_available) {
        //printf("warning/todo: gport_producer_lock(), wait for _n elements to become available\n");
        //usleep(100000);
        _p->producer_waiting = 1;
        pthread_cond_wait(&(_p->producer_data_ready),&(_p->internal_mutex));
        //printf("gport: producer received signal: data ready\n");
    }
    _p->producer_waiting = 0;

    pthread_mutex_unlock(&_p->internal_mutex);

    return _p->v + (_p->write_index)*(_p->size);
}

// unlock producer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to unlock
void gport_producer_unlock(gport _p,
                           unsigned int _n)
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

//
// consumer indirect memory access (IMA) methods
//

// consume data (indirect memory access)
//  _p      :   gport object
//  _r      :   external data buffer [size: 1 x _n]
//  _n      :   number of elements to consume (size of _r)
// returns status
int gport_consume(gport _p,
                  void * _r,
                  unsigned int _n)
{
    unsigned int num_consumed;
    unsigned int num_consumed_total = 0;

    // consume samples as they become available
    while (_n > 0 && !_p->eom) {
        // consume maximum number of samples available
        gport_consume_available(_p,
                                 _r + num_consumed_total*_p->size,
                                 _n,
                                 &num_consumed);
        // update counters
        num_consumed_total += num_consumed;
        _n -= num_consumed;
    }

    return _p->eom;
}

// produce available data (indirect memory access)
//  _p      :   gport object
//  _r      :   external data buffer [size: 1 x _nmax]
//  _nmax   :   number of elements in _r
//  _nc     :   returned number of elements in _r consumed by _p
// returns status
int gport_consume_available(gport _p,
                            void * _r,
                            unsigned int _nmax,
                            unsigned int *_nc)
{
    // lock mutexes
    pthread_mutex_lock(&(_p->consumer_mutex));
    pthread_mutex_lock(&(_p->internal_mutex));

    // wait until at least one element is available
    while (_p->num_read_elements_available == 0 && !_p->eom) {
        // set flag signaling that consumer is waiting for data
        _p->consumer_waiting = 1;

        // Block on condition that space is availble in the buffer
        // for reading (consuming). Invoking pthread_cond_wait()
        // unlocks 'internal_mutex,' allowing the producer to write
        // values to the buffer.
        pthread_cond_wait(&(_p->consumer_data_ready),
                          &(_p->internal_mutex));
    }

    // clear consumer waiting flag
    _p->consumer_waiting = 0;

    if (_p->eom) {
        pthread_mutex_unlock(&(_p->internal_mutex));
        pthread_mutex_unlock(&(_p->consumer_mutex));
        *_nc = 0;
        return _p->eom;
    }

    // set return value (number of samples we have consumed)
    unsigned int n = (_p->num_read_elements_available > _nmax) ?
        _nmax : _p->num_read_elements_available;
    *_nc = n;

    // copy data circularly if necessary
    if (n > _p->n - _p->read_index) {
        // underflow: copy data circularly
        unsigned int b = _p->n - _p->read_index;

        // copy lower section: 'b' elements
        memmove(_r,
                _p->v + (_p->read_index)*(_p->size),
                b*(_p->size));

        // copy upper section
        memmove(_r + b*(_p->size),
                _p->v,
                (n-b)*(_p->size));

    } else {
        memmove(_r,
                _p->v + (_p->read_index)*(_p->size),
                n*(_p->size));
    }

    // update internal counters
    _p->num_read_elements_available  -= n;
    _p->num_write_elements_available += n;

    // update internal read index
    _p->read_index = (_p->read_index + n) % _p->n;

    // signal producer
    if (_p->producer_waiting) {
        pthread_cond_signal(&(_p->producer_data_ready));
    }

    // release mutexes
    pthread_mutex_unlock(&(_p->internal_mutex));
    pthread_mutex_unlock(&(_p->consumer_mutex));
    return _p->eom;
}

// 
// consumer direct memory access (DMA) methods
//

// lock consumer (direct memory access)
//
//  _p      :   gport object
//  _n      :   requested number of elements to lock
//
// returns pointer to internal memory buffer for reading
void * gport_consumer_lock(gport _p,
                           unsigned int _n)
{
    // validate input
    if (_n > _p->n) {
        fprintf(stderr,"error: gport_consumer_lock(), lock requested for %u samples, exceeding buffer size\n", _n);
        exit(1);
    }

    // lock main consumer mutex: only one consumer at a time
    //printf("gport: consumer waiting for lock...\n");
    pthread_mutex_lock(&_p->consumer_mutex);
    //printf("gport: consumer locked\n");

    pthread_mutex_lock(&_p->internal_mutex);

    // TODO exit on eom flag
    while (_n > _p->num_read_elements_available) {
        //printf("warning/todo: gport_consumer_lock(), wait for _n elements to become available\n");
        //usleep(100000);
        _p->consumer_waiting = 1;
        pthread_cond_wait(&(_p->consumer_data_ready),&(_p->internal_mutex));
        //printf("gport: consumer received signal: data ready\n");
    }
    _p->consumer_waiting = 0;

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

// unlock consumer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to unlock
void gport_consumer_unlock(gport _p,
                           unsigned int _n)
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

// signal end of message (eom) flag
void gport_signal_eom(gport _p)
{
    // lock internal mutex
    pthread_mutex_lock(&(_p->internal_mutex));

    // signal eom
    _p->eom = 1;

    // producer: signal condition if waiting for data
    if (_p->producer_waiting)
        pthread_cond_signal(&(_p->producer_data_ready));

    // consumer: signal condition if waiting for data
    if (_p->consumer_waiting)
        pthread_cond_signal(&(_p->consumer_data_ready));

    // unlock internal mutex
    pthread_mutex_unlock(&(_p->internal_mutex));
}

// clear end of message (eom) flag
// TODO : test this method
void gport_clear_eom(gport _p)
{
    // lock internal mutex
    pthread_mutex_lock(&(_p->internal_mutex));

    // clear eom flag
    _p->eom = 0;

    // unlock internal mutex
    pthread_mutex_unlock(&(_p->internal_mutex));
}

