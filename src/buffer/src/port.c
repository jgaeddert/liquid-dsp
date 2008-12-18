//
//
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "metadata.h"

// Defined
//  PORT()      :   name-mangling macro
//  T           :   data type
//  BUFFER()    :   buffer macro

struct PORT(_s) {
    BUFFER() buffer;
    //metadata m;
    //char name[64];

    // mutex

};


PORT() PORT(_create)(unsigned int _n)
{
    PORT() p = (PORT()) malloc(sizeof(struct PORT(_s)));
    p->buffer = BUFFER(_create)(CIRCULAR,_n);

    // create mutex(es) here

    return p;
}

void PORT(_destroy)(PORT() _p)
{
    BUFFER(_destroy)(_p->buffer);
    free(_p);
}

void PORT(_print)(PORT() _p)
{

}

void PORT(_produce)(PORT() _p, T * _v, unsigned int _n)
{
    // lock mutex (write)
    BUFFER(_write)(_p->buffer, _v, _n);
    // unlock mutex (write)

    // 1. check to see if request for samples
    // 2. if request has been made
    //    - if request is met unlock mutex (request)
}

void PORT(_consume)(PORT() _p, T ** _v, unsigned int _n)
{
    // lock mutex (read)

    // try to read _n values :
    //   - if _n values are not available, wait
    BUFFER(_read)(_p->buffer, _v, &_n);

    // wait procedure:
    //   1. submit request for _n samples; locks mutex (request)
    //   2. wait for mutex to be unlocked
}

void PORT(_release)(PORT() _p, unsigned int _n)
{
    BUFFER(_release)(_p->buffer, _n);
    // unlock mutex (read)
}

