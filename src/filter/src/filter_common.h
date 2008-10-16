//
// 
//

#ifndef __LIQUID_FILTER_COMMON_H__
#define __LIQUID_FILTER_COMMON_H__

#define FIR_FILTER_LEN_MAX 2048

// decimator type
typedef enum {
    DECIM_GENERIC=0,
    DECIM_HALFBAND
} decim_type;

typedef enum {
    INTERP_GENERIC=0,
    INTERP_HALFBAND
} interp_type;

typedef enum {
    SQUARE=0,
    RCOS,
    RRCOS,
    W_KAISER,
} filter_prototype;

// esimate required filter length given
//   _b    : transition bandwidth (0 < _b < 0.5)
//   _slsl : sidelobe suppression level [dB]
unsigned int estimate_req_filter_len(float _b, float _slsl);

#if 0
#define VALIDATE_FILTER_LENGTH_FL(N,F,L)                                    \
    if (N>FIR_FILTER_LEN_MAX) {                                             \
        printf("warning: maximum filter length exceeded %s : %u\n", F, L);  \
        printf("  truncating to %u\n", FIR_FILTER_LEN_MAX);                 \
        N = FIR_FILTER_LEN_MAX;                                             \
        exit(0);                                                            \
    }
#define VALIDATE_FILTER_LENGTH(N) VALIDATE_FILTER_LENGTH_FL(N,__FILE__,__LINE__);
#endif

void validate_filter_length(unsigned int * _h_len);

#endif // __LIQUID_FILTER_COMMON_H__
