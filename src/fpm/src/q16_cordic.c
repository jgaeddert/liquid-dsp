// auto-generated file : do not edit
// invoked as : ./gentab/qtype_cordic_gentab -n 16 

#include "liquidfpm.internal.h"

// scaling factor 1/K:  K = prod(k=0,infty){ sqrt(1 + 2^(-2k)) }
const q16_t q16_cordic_k_inv = 0x0137;

// cordic coefficients: A[k] = arctan(2^-k)
const q16_t q16_cordic_Ak_tab[16] = {
    0x1000,
    0x0972,
    0x04fe,
    0x0289,
    0x0146,
    0x00a3,
    0x0051,
    0x0029,
    0x0014,
    0x000a,
    0x0005,
    0x0003,
    0x0001,
    0x0001,
    0x0000,
    0x0000,
};

