//
//
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "metadata.h"

// Defined
//  PORT()      :   name-mangling macro
//  T           :   data type
//  BUFFER()    :   buffer macro

struct PORT(_s) {
    gport2 gp;
};


PORT() PORT(_create)(unsigned int _n)
{
    PORT() p = (PORT()) malloc(sizeof(struct PORT(_s)));
    p->gp = gport2_create(_n,sizeof(T));

    return p;
}

void PORT(_destroy)(PORT() _p)
{
    gport2_destroy(_p->gp);
    free(_p);
}

void PORT(_print)(PORT() _p)
{
    gport2_print(_p->gp);
}

void PORT(_produce)(PORT() _p, T * _w, unsigned int _n)
{
    gport2_produce(_p->gp,(void*)_w,_n);
}

void PORT(_consume)(PORT() _p, T * _r, unsigned int _n)
{
    gport2_consume(_p->gp,(void*)_r,_n);
}


