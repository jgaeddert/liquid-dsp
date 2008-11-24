//
// Phase-locked loop
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "nco.h"

// TODO include filter_internal.h to gain direct access to
//      lf2 private members
#include "../../filter/src/filter.h"

struct pll_s {
    // loop filter
    lf2 f;
};

pll pll_create()
{
    pll p = (pll) malloc(sizeof(struct pll_s));
    
    p->f = lf2_create(0.0f);

    return p;
}

void pll_destroy(pll _p)
{
    free(_p);
}

void pll_print(pll _p)
{
    printf("  pll : bandwidth=%8.4f\n", 0.0f);
}

void pll_set_bandwidth(pll _p, float _bt)
{
    lf2_set_bandwidth(_p->f, _bt);
}

#if 0
void pll_execute(pll _p, float complex _x, float complex *_y, float _e)
{

}
#endif

void pll_step(pll _p, nco _nco, float _e)
{
    // advance loop filter
    lf2_advance(_p->f, _e, &(_nco->d_theta));
}

