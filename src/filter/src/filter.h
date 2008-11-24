//
//
//

#ifndef __LIQUID_FILTER_H__
#define __LIQUID_FILTER_H__

//
// 2nd-Order Loop Filter
//

typedef struct lf2_s * lf2;

lf2 lf2_create(float _bt);
void lf2_destroy(lf2 _f);
void lf2_print(lf2 _f);

// initialize lf2 ojbect, set to default bandwidth, reset state
void lf2_init(lf2 _f);

// set bandwidth-time product
void lf2_set_bandwidth(lf2 _f, float _bt);

// push input value and compute output
void lf2_advance(lf2 _f, float _v, float *_v_hat);

#endif // __LIQUID_FILTER_H__


