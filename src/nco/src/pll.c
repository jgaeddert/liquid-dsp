//
// Phase-locked loop
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "pll.h"
#include "../../filter/src/lf2.h"

struct pll_s {
    nco o;

    // loop filter
    lf2 f;

#if 0
    float BT;
    float beta;
    float alpha;

    float tmp2;
    float q;
    float q_hat;
    float q_prime;
#endif
};

pll pll_create()
{
    pll p = (pll) malloc(sizeof(struct pll_s));
    
    p->o = nco_create();
    p->f = lf2_create(0.01f);

    return p;
}

void pll_destroy(pll _p)
{
    free(_p);
}


