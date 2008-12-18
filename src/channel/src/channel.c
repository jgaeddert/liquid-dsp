//
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "channel.h"
#include "../../filter/src/filter.h"
#include "../../random/src/random.h"

channel channel_create()
{
    channel q = (channel) malloc(sizeof(struct channel_s));

    // generate fading filter
    //q->f_ricek = cfir_filter_create();
    q->f_ricek = NULL;

    // generate shadowing filter
    //q->f_lognorm = fir_filter_create();
    q->f_lognorm = NULL;

    return q;
}

void channel_destroy(channel _q)
{
    cfir_filter_destroy(_q->f_ricek);
    fir_filter_destroy(_q->f_lognorm);
    free(_q);
}

void channel_print(channel _q)
{

}

void channel_execute(channel _q, float complex _x, float complex *_y)
{
    float complex r, x, y;
    float z;

    // advance fading filter
    r = crandnf();
    cfir_filter_push(_q->f_ricek, r);
    cfir_filter_execute(_q->f_ricek, &x);

    // TODO: compensate for filter

    // generate complex fading envelope
    y = _Complex_I*( crealf(x)*(_q->sig) + _q->s ) +
                   ( cimagf(x)*(_q->sig)         );

    // advance shadowing filter
    r = randnf();
    fir_filter_push(_q->f_lognorm, r);
    fir_filter_execute(_q->f_lognorm, &z);


    // additive white gaussian noise
    float complex n;
    n = crandnf();  // TODO: noise power?

    // set return value
    *_y = (_x * y * z) + n;
}

