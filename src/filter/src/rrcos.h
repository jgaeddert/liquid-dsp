//
// Design root raised-cosine filter
//

#ifndef __LIQUID_RRCOS_H__
#define __LIQUID_RRCOS_H__

void design_rrc_filter(
    unsigned int k,      // samples per symbol
    unsigned int m,      // delay
    float beta,          // rolloff factor ( 0 < beta <= 1 )
    float dt,
    float * h            // pointer to filter coefficients
);

#endif /* __LIQUID_RRCOS_H__ */
