//
// Least mean-squares equalizer
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define DEBUG

struct EQLMS(_s) {
    unsigned int p;     // filter order
    float mu;           // LMS step size

    // internal matrices
    T * w0, * w1;       // weights [px1]

    unsigned int n;     // input counter
    WINDOW() buffer;    // input buffer
};

EQLMS() EQLMS(_create)(unsigned int _p)
{
    EQLMS() eq = (EQLMS()) malloc(sizeof(struct EQLMS(_s)));

    // set filter order, other params
    eq->p = _p;
    eq->mu = 0.1f;
    eq->n=0;

    eq->w0 = (T*) malloc((eq->p)*sizeof(T));
    eq->w1 = (T*) malloc((eq->p)*sizeof(T));
    eq->buffer = WINDOW(_create)(eq->p);

    EQLMS(_reset)(eq);

    return eq;
}

void EQLMS(_destroy)(EQLMS() _eq)
{
    free(_eq->w0);
    free(_eq->w1);

    WINDOW(_destroy)(_eq->buffer);
    free(_eq);
}

void EQLMS(_print)(EQLMS() _eq)
{
    printf("equalizer (LMS):\n");
    printf("    order:      %u\n", _eq->p);
}

void EQLMS(_reset)(EQLMS() _eq)
{
    unsigned int i;
    for (i=0; i<_eq->p; i++)
        _eq->w0[i] = 0;

    WINDOW(_clear)(_eq->buffer);
}

//
//  _x  :   received sample
//  _d  :   desired output
//  _w  :   output weights
void EQLMS(_execute)(EQLMS() _eq, T _x, T _d, T * _d_hat)
{
    unsigned int i;
    unsigned int p=_eq->p;

    // push value into buffer
    WINDOW(_push)(_eq->buffer, _x);
    T * x;

    // check to see if buffer is full, return if not
    _eq->n++;
    if (_eq->n < _eq->p)
        return;

    // compute d_hat (dot product, estimated output)
    T d_hat = 0;
    WINDOW(_read)(_eq->buffer, &x);
    //DOTPROD(_run)(_eq->w0, x, p, &d_hat);
    for (i=0; i<p; i++)
        d_hat += conj(_eq->w0[i])*x[i];
    *_d_hat = d_hat;

    // compute error (a priori)
    T alpha = _d - d_hat;

    // update weighting vector
    // w[n+1] = w[n] + mu*conj(d-d_hat)*x[n]
    for (i=0; i<p; i++)
        _eq->w1[i] = _eq->w0[i] + (_eq->mu)*conj(alpha)*x[i];

#ifdef DEBUG
    printf("w0: \n");
    for (i=0; i<p; i++) {
        PRINTVAL(_eq->w0[i]);
        printf("\n");
    }
    printf("w1: \n");
    for (i=0; i<p; i++) {
        PRINTVAL(_eq->w1[i]);
        printf("\n");
    }
#endif

    // copy old values
    memmove(_eq->w0, _eq->w1,   p*sizeof(T));
}

//
//  _w  :   initial weights / output weights
//  _x  :   received sample vector
//  _d  :   desired output vector
//  _n  :   vector length
void EQLMS(_train)(EQLMS() _eq, T * _w, T * _x, T * _d, unsigned int _n)
{
    unsigned int i, p=_eq->p;
    if (_n < p) {
        printf("warning: eqlms_xxxt_train(), traning sequence less than filter order\n");
        return;
    }

    // reset equalizer state
    EQLMS(_reset)(_eq);

    // copy initial weights into buffer
    for (i=0; i<p; i++)
        _eq->w0[i] = _w[p - i - 1];

    T d_hat;
    for (i=0; i<_n; i++)
        EQLMS(_execute)(_eq, _x[i], _d[i], &d_hat);

    // copy output weight vector...
    for (i=0; i<p; i++)
        _w[i] = _eq->w1[p-i-1];
}
