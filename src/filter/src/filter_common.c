//
// 
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "filter_common.h"

// esimate required filter length given
//   _b    : transition bandwidth (0 < b < 0.5)
//   _slsl : sidelobe suppression level [dB]
unsigned int estimate_req_filter_len(float _b, float _slsl)
{
    if (_b >= 0.5f || _b <= 0.0f) {
        printf("error: estimate_req_filter_len(), invalid bandwidth : %f\n", _b);
        exit(0);
    }

    if (_slsl <= 0.0f) {
        printf("error: estimate_req_filter_len(), invalid sidelobe level : %f\n", _slsl);
        exit(0);
    }

    unsigned int h_len = (unsigned int) round((_slsl-8)/(14*_b));
    validate_filter_length(&h_len);
    return h_len;
}

void validate_filter_length(unsigned int * _h_len)
{
    if (*_h_len > FIR_FILTER_LEN_MAX) {
        printf("warning: maximum filter length exceeded, truncating to %u\n",
                FIR_FILTER_LEN_MAX);
        *_h_len = FIR_FILTER_LEN_MAX;
    } else if (*_h_len < 2) {
        printf("warning: minimum filter length exceeded, extending to 2\n");
        *_h_len = 2;
    }
}


