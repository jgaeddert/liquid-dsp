//
// Generic dot product
//

#include <stdlib.h>
#include <string.h>

struct X(_s) {
    TC * h;
    unsigned int n;
};

// basic dot product

TO X(_run)(TC *_h, TI *_x, unsigned int _n)
{
    TO r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    return r;
}

TO X(_run4)(TC *_h, TI *_x, unsigned int _n)
{
    TO r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _h[i] * _x[i];

    return r;
}

//
// structured dot product
//

X() X(_create)(TC * _h, unsigned int _n)
{
    X() q = (X()) malloc(sizeof(struct X(_s)));
    q->n = _n;
    q->h = (TC*) malloc((q->n)*sizeof(TC));
    memmove(q->h, _h, (q->n)*sizeof(TC));
    return q;
}

void X(_destroy)(X() _q)
{
    free(_q->h);
    free(_q);
}

TO X(_execute)(X() _q, TI * _x)
{
    return X(_run)(_q->h, _x, _q->n);
}

