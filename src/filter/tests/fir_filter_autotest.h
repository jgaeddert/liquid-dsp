#ifndef __FIR_FILTER_AUTOTEST_H__
#define __FIR_FILTER_AUTOTEST_H__

#include "../src/fir_filter.h"

void autotest_create_external_coeff_01() {

    // Initialize pre-determined coefficient array
    float h0[8] = { 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f };

    // Create filter
    fir_filter f = fir_filter_create(h0, 8);

    // Assert arrays are the same length
    CONTEND_EQUALITY( f->h_len, 8 );

    // Ensure data are equal
    //CONTEND_SAME_DATA( f->h, h0, 8*sizeof(float) );

    fir_filter_destroy(f);
}

void autotest_impulse_response() {

    // Initialize variables
    float h[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    float y;        // output

    // Load filter coefficients externally
    fir_filter f = fir_filter_create(h, 10);

    CONTEND_EQUALITY( f->h_len, 10 );

    // Create circular buffer
    fbuffer cbuf = fbuffer_create(CIRCULAR, 10);
    float * buf; // linearlized buffer

    // Hit the filter with an impulse
    fbuffer_zero(cbuf);
    fbuffer_push(cbuf, 1.0f);

    unsigned int i, n;
    // Resulting output should be equal to filter coefficients
    for (i=0; i<10; i++) {
        n = 10;
        fbuffer_read(cbuf, &buf, &n);
        y = fir_filter_execute(f, buf);
        CONTEND_EQUALITY( y, h[i]);
        fbuffer_push(cbuf, 0.0f);
    }

    // Impulse response should be finite
    for (i=0; i<10; i++) {
        n = 10;
        fbuffer_read(cbuf, &buf, &n);
        y = fir_filter_execute(f, buf);
        CONTEND_DELTA( 0.0f, y, 0.001 );
        fbuffer_push(cbuf, 0.0f);
    }

    fir_filter_destroy(f);
    fbuffer_destroy(cbuf);
}

void autotest_matched_response() {

    // Initialize variables
    float h[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    float y;        // output
    float test[] = {
       0.10000,
       0.29000,
       0.56000,
       0.90000,
       1.30000,
       1.75000,
       2.24000,
       2.76000,
       3.30000,
       3.85000,
       3.30000,
       2.76000,
       2.24000,
       1.75000,
       1.30000,
       0.90000,
       0.56000,
       0.29000,
       0.10000
    };

    // Load filter coefficients externally
    fir_filter f = fir_filter_create(h, 10);

    CONTEND_EQUALITY( f->h_len, 10 );

    // Create circular buffer
    fbuffer cbuf = fbuffer_create(CIRCULAR, 10);
    float * buf; // linearlized buffer

    fbuffer_zero(cbuf);

    unsigned int i, n;
    // Resulting output should be equal to filter coefficients
    for (i=0; i<10; i++) {
        n = 10;
        fbuffer_read(cbuf, &buf, &n);
        y = fir_filter_execute(f, buf);
        printf("h[%d] = %f\n", i, y);
        fbuffer_push(cbuf, h[i]);
    }

    if (false) {
    // Impulse response should be finite
    for (i=0; i<10; i++) {
        n = 10;
        fbuffer_read(cbuf, &buf, &n);
        y = fir_filter_execute(f, buf);
        CONTEND_DELTA( 0.0f, y, 0.001 );
        fbuffer_push(cbuf, 0.0f);
    }
    }

    fir_filter_destroy(f);
    fbuffer_destroy(cbuf);
}


#endif // __FIR_FILTER_AUTOTEST_H__

