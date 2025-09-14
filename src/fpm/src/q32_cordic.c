// auto-generated file : do not edit
// invoked as : ./gentab/qtype_cordic_gentab -n 32 

#include "liquidfpm.internal.h"

// scaling factor 1/K:  K = prod(k=0,infty){ sqrt(1 + 2^(-2k)) }
const q32_t q32_cordic_k_inv = 0x0136e9dc;

// cordic coefficients: A[k] = arctan(2^-k)
const q32_t q32_cordic_Ak_tab[32] = {
    0x10000000,
    0x09720290,
    0x04fd9c30,
    0x028888e8,
    0x014586a2,
    0x00a2ebf0,
    0x00517b0f,
    0x0028be2b,
    0x00145f2a,
    0x000a2f97,
    0x000517cc,
    0x00028be6,
    0x000145f3,
    0x0000a2fa,
    0x0000517d,
    0x000028be,
    0x0000145f,
    0x00000a30,
    0x00000518,
    0x0000028c,
    0x00000146,
    0x000000a3,
    0x00000051,
    0x00000029,
    0x00000014,
    0x0000000a,
    0x00000005,
    0x00000003,
    0x00000001,
    0x00000001,
    0x00000000,
    0x00000000,
};

