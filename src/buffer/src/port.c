//
//
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "port.h"
#include "metadata.h"
#include "buffer.h"

struct port_s {
    fbuffer buffer;
    //metadata m;
    //char name[64];

    // mutex

};


port port_create(unsigned int _n)
{
    port p = (port) malloc(sizeof(struct port_s));
    p->buffer = fbuffer_create(CIRCULAR,_n);

    // create mutex(es) here

    return p;
}

void port_destroy(port _p)
{
    fbuffer_destroy(_p->buffer);
    free(_p);
}

void port_print(port _p)
{

}

void port_produce(port _p, float * _v, unsigned int _n)
{
    // lock mutex (write)
    fbuffer_write(_p->buffer, _v, _n);
    // unlock mutex (write)

    // 1. check to see if request for samples
    // 2. if request has been made
    //    - if request is met unlock mutex (request)
}

void port_consume(port _p, float ** _v, unsigned int _n)
{
    // lock mutex (read)

    // try to read _n values :
    //   - if _n values are not available, wait
    fbuffer_read(_p->buffer, _v, &_n);

    // wait procedure:
    //   1. submit request for _n samples; locks mutex (request)
    //   2. wait for mutex to be unlocked
}

void port_release(port _p, unsigned int _n)
{
    fbuffer_release(_p->buffer, _n);
    // unlock mutex (read)
}

