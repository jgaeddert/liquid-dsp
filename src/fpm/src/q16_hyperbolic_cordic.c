// auto-generated file : do not edit
// invoked as : ./gentab/qtype_hyperbolic_cordic_gentab -n 16 

#include "liquidfpm.internal.h"

// scaling factor 1/Kp:  
//   Kp = prod(k=0,infty){ sqrt(1 - 2^(-2*k)) } *
//        prod(k=0,infty){ sqrt(1 - 2^(-2*i_k)) }
// where
//   i_0     = 4,
//   i_(k+1) = 3*i_k + 1
// 1/Kp = 1.20749711990356
const q16_t q16_sinhcosh_cordic_kp_inv = 0x026a;

// sinh|cosh cordic coefficients: A[k] = arctanh(2^-k)
// NOTE: first value is ignored in algorithm
const q16_t q16_sinhcosh_cordic_Ak_tab[16] = {
    0x0000,
    0x0119,
    0x0083,
    0x0040,
    0x0020,
    0x0010,
    0x0008,
    0x0004,
    0x0002,
    0x0001,
    0x0001,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000};

