/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Phase-locked loop
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

struct pll_s {
    // loop filter
    lf2 f;
};

pll pll_create()
{
    pll p = (pll) malloc(sizeof(struct pll_s));
    
    p->f = lf2_create(0.0f);
    lf2_set_damping_factor(p->f, 2.5f);

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

void pll_reset(pll _p)
{
    lf2_reset(_p->f);
}

void pll_set_bandwidth(pll _p, float _bt)
{
    lf2_set_bandwidth(_p->f, _bt);
}


void pll_set_damping_factor(pll _p, float _xi)
{
    lf2_set_damping_factor(_p->f, _xi);
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

