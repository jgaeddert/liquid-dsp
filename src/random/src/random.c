//
//
//

#include <math.h>
#include "random.h"

void randnf(float * i, float * q)
{
    // generate two uniform random numbers
    float u1, u2;

    // ensure u1 does not equal zero
    do {
        u1 = randf();
    } while (u1 == 0.0f);

    u2 = randf();

    float x = sqrtf(-2*logf(u1));
    *i = x * sinf(2*M_PI*u2);
    *q = x * cosf(2*M_PI*u2);
}


