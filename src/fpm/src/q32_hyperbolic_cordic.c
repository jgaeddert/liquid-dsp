// auto-generated file : do not edit
// invoked as : ./gentab/qtype_hyperbolic_cordic_gentab -n 32 

#include "liquidfpm.internal.h"

// scaling factor 1/Kp:  
//   Kp = prod(k=0,infty){ sqrt(1 - 2^(-2*k)) } *
//        prod(k=0,infty){ sqrt(1 - 2^(-2*i_k)) }
// where
//   i_0     = 4,
//   i_(k+1) = 3*i_k + 1
// 1/Kp = 1.20749711990356
const q32_t q32_sinhcosh_cordic_kp_inv = 0x026a3d10;

// sinh|cosh cordic coefficients: A[k] = arctanh(2^-k)
// NOTE: first value is ignored in algorithm
const q32_t q32_sinhcosh_cordic_Ak_tab[32] = {
    0x00000000,
    0x01193ea8,
    0x0082c578,
    0x00405625,
    0x00200ab1,
    0x00100156,
    0x0008002b,
    0x00040005,
    0x00020001,
    0x00010000,
    0x00008000,
    0x00004000,
    0x00002000,
    0x00001000,
    0x00000800,
    0x00000400,
    0x00000200,
    0x00000100,
    0x00000080,
    0x00000040,
    0x00000020,
    0x00000010,
    0x00000008,
    0x00000004,
    0x00000002,
    0x00000001,
    0x00000001,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000};

