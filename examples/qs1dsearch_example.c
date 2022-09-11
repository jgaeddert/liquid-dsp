// quad-section search in one dimension
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

float utility(float _v, void * _context)
{
    float v_opt = *(float*)(_context);
    float v = _v - v_opt;
    return tanhf(v)*tanhf(v);
}
//  { return _v*_v; }
//  { return tanhf(_v)*tanhf(_v) + 0.001*_v*_v; }

int main()
{
    // create qs1dsearch object
    float v_opt  = 3.0f;
    qs1dsearch q = qs1dsearch_create(utility, &v_opt, LIQUID_OPTIM_MINIMIZE);

    //qs1dsearch_init_bounds(q, -20, 10);
    qs1dsearch_init(q, -20);

    unsigned int i;
    for (i=0; i<20; i++) {
        qs1dsearch_step(q);
    }
    printf("%3u : u(%12.8f) = %12.4e\n", i,
        qs1dsearch_get_opt_v(q),
        qs1dsearch_get_opt_u(q));

    qs1dsearch_destroy(q);

    return 0;
}
