//
// Windows, defined by macro
//

#include "liquid.internal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct WINDOW(_s) {
    T * v;                      // allocated array pointer
    unsigned int len;           // length of window
    unsigned int m;             // floor(log2(len)) + 1
    unsigned int n;             // 2^m
    unsigned int mask;          // n-1
    unsigned int N;             // number of elements allocated
                                // in memory
    unsigned int read_index;
};

WINDOW() WINDOW(_create)(unsigned int _n)
{
    WINDOW() w = (WINDOW()) malloc(sizeof(struct WINDOW(_s)));
    w->len = _n;

    w->m = msb_index(_n);   // effectively floor(log2(len))+1
    w->n = 1<<(w->m);       // 
    w->mask = w->n - 1;     // bit mask

    // number of elements to allocate to memory
    w->N = w->n + w->len - 1;

    // allocte memory
    w->v = (T*) malloc((w->N)*sizeof(T));
    w->read_index = 0;

    // clear window
    WINDOW(_clear)(w);

    return w;
}

void WINDOW(_destroy)(WINDOW() _w)
{
    free(_w->v);
    free(_w);
}

void WINDOW(_print)(WINDOW() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    for (i=0; i<_w->len; i++) {
        printf("%u", i);
        BUFFER_PRINT_LINE(_w,(_w->read_index+i)%(_w->len))
        printf("\n");
    }
}

void WINDOW(_debug_print)(WINDOW() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    for (i=0; i<_w->len; i++) {
        // print read index pointer
        if (i==_w->read_index)
            printf("<r>");

        // print window value
        BUFFER_PRINT_LINE(_w,i)
        printf("\n");
    }
    printf("----------------------------------\n");

    // print excess window memory
    for (i=_w->len; i<_w->N; i++) {
        BUFFER_PRINT_LINE(_w,i)
        printf("\n");
    }
}

void WINDOW(_clear)(WINDOW() _w)
{
    _w->read_index = 0;
    memset(_w->v, 0, (_w->len)*sizeof(T));
}

void WINDOW(_read)(WINDOW() _w, T ** _v)
{
    // return pointer to buffer
    *_v = _w->v + _w->read_index;
}

void WINDOW(_push)(WINDOW() _w, T _v)
{
    // increment index
    _w->read_index++;

    // wrap around pointer
    _w->read_index &= _w->mask;

    // if pointer wraps around, copy excess memory
    if (_w->read_index == 0)
        memmove(_w->v, _w->v + _w->n, (_w->len)*sizeof(T));

    // append value to end of buffer
    _w->v[_w->read_index + _w->len - 1] = _v;
}

void WINDOW(_write)(WINDOW() _w, T * _v, unsigned int _n)
{
    // TODO make this more efficient
    unsigned int i;
    for (i=0; i<_n; i++)
        WINDOW(_push)(_w, _v[i]);
}

