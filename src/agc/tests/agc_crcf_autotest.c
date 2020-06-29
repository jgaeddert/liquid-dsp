/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// Test DC gain control
//
void autotest_agc_crcf_dc_gain_control()
{
    // set paramaters
    float gamma = 0.1f;     // nominal signal level
    float bt    = 0.1f;     // bandwidth-time product
    float tol   = 0.001f;   // error tolerance

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x = gamma;    // input sample
    float complex y;            // output sample
    for (i=0; i<256; i++)
        agc_crcf_execute(q, x, &y);
    
    // Check results
    CONTEND_DELTA( crealf(y), 1.0f, tol );
    CONTEND_DELTA( cimagf(y), 0.0f, tol );
    CONTEND_DELTA( agc_crcf_get_gain(q), 1.0f/gamma, tol );

    // destroy AGC object
    agc_crcf_destroy(q);
}

// 
// Test AC gain control
//
void autotest_agc_crcf_ac_gain_control()
{
    // set paramaters
    float gamma = 0.1f;             // nominal signal level
    float bt    = 0.1f;             // bandwidth-time product
    float tol   = 0.001f;           // error tolerance
    float dphi  = 0.1f;             // NCO frequency

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x;
    float complex y;
    for (i=0; i<256; i++) {
        x = gamma * cexpf(_Complex_I*i*dphi);
        agc_crcf_execute(q, x, &y);
    }

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, agc_crcf_get_signal_level(q));

    // Check results
    CONTEND_DELTA( agc_crcf_get_gain(q), 1.0f/gamma, tol);

    // destroy AGC object
    agc_crcf_destroy(q);
}



// 
// Test RSSI on sinusoidal input
//
void autotest_agc_crcf_rssi_sinusoid()
{
    // set paramaters
    float gamma = 0.3f;         // nominal signal level
    float bt    = 0.05f;        // agc bandwidth
    float tol   = 0.001f;       // error tolerance

    // signal properties
    float dphi = 0.1f;          // signal frequency

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<512; i++) {
        // generate sample (complex sinusoid)
        x = gamma * cexpf(_Complex_I*dphi*i);

        // execute agc
        agc_crcf_execute(q, x, &y);
    }

    // get received signal strength indication
    float rssi = agc_crcf_get_signal_level(q);

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, rssi);

    // Check results
    CONTEND_DELTA( rssi, gamma, tol );

    // destroy agc object
    agc_crcf_destroy(q);
}


// 
// Test RSSI on noise input
//
void autotest_agc_crcf_rssi_noise()
{
    // set paramaters
    float gamma = -30.0f;   // nominal signal level [dB]
    float bt    =  2e-3f;   // agc bandwidth
    float tol   =  1.0f;    // error tolerance [dB]

    // signal properties
    float nstd = powf(10.0f, gamma/20);

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<8000; i++) {
        // generate sample (circular complex noise)
        x = nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // execute agc
        agc_crcf_execute(q, x, &y);
    }

    // get received signal strength indication
    float rssi = agc_crcf_get_rssi(q);

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, rssi);

    // Check results
    CONTEND_DELTA( rssi, gamma, tol );

    // destroy agc object
    agc_crcf_destroy(q);
}

// 
// Test squelch functionality
//
void autotest_agc_crcf_squelch()
{
    // create agc object, set loop bandwidth, and initialize parameters
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, 0.25);
    agc_crcf_set_signal_level(q,1e-3f);     // initial guess at starting signal level

    // initialize squelch functionality
    agc_crcf_squelch_enable(q);             // enable squelch
    agc_crcf_squelch_set_threshold(q, -50); // threshold for detection [dB]
    agc_crcf_squelch_set_timeout  (q, 100); // timeout for hysteresis

    // run agc
    unsigned int num_samples = 2000; // total number of samples to run
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate signal, applying tapering window appropriately
        float gamma = 0.0f;
        if      (i <  500) gamma = 1e-3f;
        else if (i <  550) gamma = 1e-3f + (1e-2f - 1e-3f)*(0.5f - 0.5f*cosf(M_PI*(float)(i- 500)/50.0f));
        else if (i < 1450) gamma = 1e-2f;
        else if (i < 1500) gamma = 1e-3f + (1e-2f - 1e-3f)*(0.5f + 0.5f*cosf(M_PI*(float)(i-1450)/50.0f));
        else               gamma = 1e-3f;
        float complex x = gamma * cexpf(_Complex_I*2*M_PI*0.0193f*i);

        // apply gain
        float complex y;
        agc_crcf_execute(q, x, &y);

        // retrieve signal level [dB]
        //rssi = agc_crcf_get_rssi(q);

        // get squelch mode
        int mode = agc_crcf_squelch_get_status(q);

        // check certain conditions based on sample input (assuming 2000 samples)
        switch (i) {
            case    0: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_ENABLED);  break;
            case  500: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_ENABLED);  break;
            case  600: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_SIGNALHI); break;
            case 1400: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_SIGNALHI); break;
            case 1500: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_SIGNALLO); break;
            case 1600: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_ENABLED);  break;
            case 1900: CONTEND_EQUALITY(mode, LIQUID_AGC_SQUELCH_ENABLED);  break;
            default:;
        }
    }

    // destroy AGC object
    agc_crcf_destroy(q);
}



