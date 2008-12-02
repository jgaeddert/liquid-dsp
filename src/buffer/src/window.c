//
// Windows, defined by macro
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "window_internal.h"

struct X(_s) {
    T * v;                      // allocated array
    unsigned int len;           // length of window
    unsigned int N;             // number of elements allocated
                                // in memory
    unsigned int read_index;
    //T * v_excess;               // excess memory pointer
};

X() X(_create)(unsigned int _n)
{
    X() w = (X()) malloc(sizeof(struct X(_s)));
    w->len = _n;

    w->N = 2*(w->len) - 1;

    w->v = (T*) malloc((w->N)*sizeof(T));
    w->read_index = 0;

    return w;
}

void X(_destroy)(X() _w)
{
    free(_w->v);
    free(_w);
}

void X(_print)(X() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    for (i=0; i<_w->len; i++) {
        printf("%u", i);
        WINDOW_PRINT_LINE(_w,(_w->read_index+i)%(_w->len))
        printf("\n");
    }
}

void X(_debug_print)(X() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    for (i=0; i<_w->len; i++) {
        // print read index pointer
        if (i==_w->read_index)
            printf("<r>");

        // print window value
        WINDOW_PRINT_LINE(_w,i)
        printf("\n");
    }
    printf("----------------------------------\n");

    // print excess window memory
    for (i=_w->len; i<_w->N; i++) {
        WINDOW_PRINT_LINE(_w,i)
        printf("\n");
    }
}

void X(_clear)(X() _w)
{
    _w->read_index = 0;
    memset(_w->v, 0, (_w->len)*sizeof(T));
}

void X(_read)(X() _w, T ** _v)
{
    if (_w->read_index > 0) {
        //
        X(_linearize)(_w);
    }
    *_v = _w->v + _w->read_index;
}

void X(_push)(X() _w, T _v)
{
    _w->v[_w->read_index] = _v;
    _w->read_index = (_w->read_index+1) % _w->len;
}

void X(_write)(X() _w, T * _v, unsigned int _n)
{
    // TODO make this more efficient
    unsigned int i;
    for (i=0; i<_n; i++)
        X(_push)(_w, _v[i]);
}

void X(_linearize)(X() _w)
{
    // TODO check to see if anything needs to be done

    // perform memory copy
    memcpy(_w->v + _w->len, _w->v, (_w->read_index)*sizeof(T));
}

