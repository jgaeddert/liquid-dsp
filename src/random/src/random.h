//
// Random number generators
//

#ifndef __LIQUID_RANDOM_H__
#define __LIQUID_RANDOM_H__

#include <stdlib.h>

// Uniform random number generator, (0,1]
inline static float randf() { return (float) rand() / (float) RAND_MAX; };

// Gaussian random number generator, N(0,1)
void randnf(float * i, float * q);

#endif /* __LIQUID_RANDOM_H__ */

