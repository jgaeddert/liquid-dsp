/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// test ASCII spectral periodogram (asgram) objects

#include <stdlib.h>
#include "liquid.internal.h"
#include "liquid.autotest.h"

LIQUID_AUTOTEST(asgramcf_copy,"check copy method","",0.1)
{
    // options
    unsigned int nfft = 70; // transform size
    unsigned int num_samples = 844;
    float nstd = 0.1f;

    // create object
    asgramcf q0 = asgramcf_create(nfft);

    // set parameters
    asgramcf_set_scale  (q0, -20.8f, 3.1f);
    asgramcf_set_display(q0, "abcdeFGHIJ");

    // generate a bunch of random noise samples
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        float complex v = 0.1f + nstd * (randnf() + _Complex_I*randnf());
        asgramcf_push(q0, v);
    }

    // copy object and push same samples through both
    asgramcf q1 = asgramcf_copy(q0);
    asgramcf_autoscale_enable(q0);
    asgramcf_autoscale_enable(q1);
    for (i=0; i<num_samples; i++) {
        float complex v = 0.1f + nstd * (randnf() + _Complex_I*randnf());
        asgramcf_push(q0, v);
        asgramcf_push(q1, v);
    }

    // get spectrum and compare outputs
    char a0[nfft], a1[nfft];
    float pv0, pv1, pf0, pf1;
    asgramcf_execute(q0, a0, &pv0, &pf0);
    asgramcf_execute(q1, a1, &pv1, &pf1);
    LIQUID_CHECK_ARRAY(a0, a1, nfft*sizeof(char));
    LIQUID_CHECK      (pv0 == pv1);
    LIQUID_CHECK      (pf0 == pf1);

    // destroy objects
    asgramcf_destroy(q0);
    asgramcf_destroy(q1);
}

LIQUID_AUTOTEST(asgramcf_config,"check both valid and invalid configurations","",0.1)
{
    _liquid_error_downgrade_enable();

    // check that object returns NULL for invalid configurations
    LIQUID_CHECK( NULL == asgramcf_create(0)); // nfft too small
    LIQUID_CHECK( NULL == asgramcf_create(1)); // nfft too small

    // create proper object and test configuration
    asgramcf q = asgramcf_create(64);

    LIQUID_CHECK( LIQUID_OK == asgramcf_autoscale_enable(q));
    LIQUID_CHECK( LIQUID_OK == asgramcf_autoscale_disable(q));

    asgramcf_destroy(q);
    _liquid_error_downgrade_disable();
}

