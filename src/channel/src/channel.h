//
//
//

#ifndef __LIQUID_CHANNEL_H__
#define __LIQUID_CHANNEL_H__

#include <complex.h>

//
// AWGN channel
//
typedef struct awgn_channel_s * awgn_channel;
awgn_channel awgn_channel_create(float _nvar);
void awgn_channel_destroy(awgn_channel _q);
void awgn_channel_print(awgn_channel _q);
void awgn_channel_execute(awgn_channel _q, float complex _x, float complex *_y);
void awgn_channel_set_noise_variance(awgn_channel _q, float _nvar);

// 
// Rice-K channel
//
typedef struct ricek_channel_s * ricek_channel;
ricek_channel ricek_channel_create(unsigned int _h_len, float _K, float _fd, float _theta);
void ricek_channel_destroy(ricek_channel _q);
void ricek_channel_print(ricek_channel _q);
void ricek_channel_execute(ricek_channel _q, float complex _x, float complex *_y);

//
// Log-normal channel
//
typedef struct lognorm_channel_s * lognorm_channel;

// 
// Composite channel
//
typedef struct channel_s * channel;
channel channel_create();
void channel_destroy(channel _c);
void channel_print(channel _c);
void channel_execute(channel _c, float complex _x, float complex *_y);

#endif // __LIQUID_CHANNEL_H__

