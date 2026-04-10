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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.internal.h"
#include "liquid.autotest.h"

// peak callback function; value nearest {p, p, p, ...} where p = 1/sqrt(2)
float gasearch_autotest_peak_callback(void * _userdata, chromosome _c)
{
    unsigned int i, n = chromosome_get_num_traits(_c);
    float u     = 1.0f;
    float sig   = 0.2f;
    float p     = M_SQRT1_2;
    for (i=0; i<n; i++) {
        // extract chromosome values
        float v = chromosome_valuef(_c,i);
        float e = v - p;
        u *= exp(-e*e/(2*sig*sig));
    }
    return u;
}

LIQUID_AUTOTEST(gasearch_peak,"find values which maximize function","",0.1)
{
    unsigned int num_parameters     = 16;   // dimensionality of search (minimum 1)
    unsigned int bits_per_parameter =  6;   // parameter resolution
    unsigned int num_iterations     = 8000; // number of iterations to run
    unsigned int population_size    = 16;   // GA population size
    float        mutation_rate      = 0.2f; // GA mutation rate
    float        tol                = 0.1f; // error tolerance

    unsigned int i;
    float optimum_utility;

    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(num_parameters, bits_per_parameter);

    // create gasearch object
    gasearch ga = gasearch_create_advanced(gasearch_autotest_peak_callback, NULL, prototype,
                    LIQUID_OPTIM_MAXIMIZE, population_size, mutation_rate);

    // execute search at once
    optimum_utility = gasearch_run(ga, num_iterations, 1e6f);

    // get optimum utility
    gasearch_getopt(ga, prototype, &optimum_utility);
    LIQUID_VLA(float, v_opt, num_parameters);
    for (i=0; i<num_parameters; i++)
        v_opt[i] = chromosome_valuef(prototype, i);

    // destroy search object
    chromosome_destroy(prototype);
    gasearch_destroy(ga);

    // test results, optimum at {p, p, p, ...} where p = 1/sqrt(2)
    liquid_log_debug("opt: [%12.6f]", optimum_utility);
    for (i=0; i<num_parameters; i++) {
        liquid_log_debug("  v_opt[%2u] = %12.8f", i, v_opt[i]);
        LIQUID_CHECK_DELTA(v_opt[i], M_SQRT1_2, tol)
    }

    // test value of utility (should be nearly 1)
    LIQUID_CHECK( optimum_utility> 0.70f )
}

LIQUID_AUTOTEST(chromosome_config,"test chromosome configuration","",0.1)
{
    _liquid_error_downgrade_enable();
    // test chromosome
    unsigned int bits_per_trait_invalid[8] = {6,6,6,6,6,6,6,1000};
    unsigned int bits_per_trait_valid  [8] = {6,6,6,6,6,6,6,  32};
    LIQUID_CHECK(NULL ==chromosome_create(bits_per_trait_invalid, 8))
    LIQUID_CHECK(NULL ==chromosome_create(bits_per_trait_valid,   0))
    LIQUID_CHECK(NULL ==chromosome_create_basic(0, 12)) // too few traits
    LIQUID_CHECK(NULL ==chromosome_create_basic(8,  0)) // bits per trait too small
    LIQUID_CHECK(NULL ==chromosome_create_basic(8, 99)) // bits per trait too large

    // create prototype chromosome using basic method
    chromosome prototype = chromosome_create_basic(20, 5);
    LIQUID_CHECK(LIQUID_OK == chromosome_print(prototype))
    chromosome_destroy(prototype);

    // create prototype chromosome using more specific method
    prototype = chromosome_create(bits_per_trait_valid, 8);
    LIQUID_CHECK  (LIQUID_OK == chromosome_print    (prototype))
    LIQUID_CHECK  (LIQUID_OK == chromosome_reset    (prototype))

    // test initialization
    unsigned int values_invalid[] = {999,12,11,13,63,17, 3,123456789}; // invalid because first trait is only 6 bits
    unsigned int values_valid  [] = {  0,12,11,13,63,17, 3,123456789};
    LIQUID_CHECK(LIQUID_OK != chromosome_init (prototype, values_invalid))
    LIQUID_CHECK(LIQUID_OK == chromosome_init (prototype, values_valid  ))
    LIQUID_CHECK(        0 == chromosome_value    (prototype,999))
    LIQUID_CHECK(     0.0f == chromosome_valuef   (prototype,999))
    LIQUID_CHECK(LIQUID_OK != chromosome_mutate   (prototype,999))
    LIQUID_CHECK(LIQUID_OK != chromosome_crossover(prototype,prototype,prototype,999))

    // check individual values
    LIQUID_CHECK( chromosome_value(prototype, 0) ==          0)
    LIQUID_CHECK( chromosome_value(prototype, 1) ==         12)
    LIQUID_CHECK( chromosome_value(prototype, 2) ==         11)
    LIQUID_CHECK( chromosome_value(prototype, 3) ==         13)
    LIQUID_CHECK( chromosome_value(prototype, 4) ==         63)
    LIQUID_CHECK( chromosome_value(prototype, 5) ==         17)
    LIQUID_CHECK( chromosome_value(prototype, 6) ==          3)
    LIQUID_CHECK( chromosome_value(prototype, 7) ==  123456789)

    // test initialization (float values)
    float valuesf_invalid[] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,999,};
    float valuesf_valid  [] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,};
    LIQUID_CHECK(LIQUID_OK != chromosome_initf(prototype, valuesf_invalid))
    LIQUID_CHECK  (LIQUID_OK == chromosome_initf(prototype, valuesf_valid  ))

    // check individual values
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 0), 0.0f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 1), 0.1f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 2), 0.2f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 3), 0.3f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 4), 0.4f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 5), 0.5f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 6), 0.6f, 0.02f )
    LIQUID_CHECK_DELTA( chromosome_valuef(prototype, 7), 0.7f, 0.02f )

    // destroy objects
    chromosome_destroy(prototype);
    _liquid_error_downgrade_disable();
}

LIQUID_AUTOTEST(gasearch_config,"gasearch configuration","",0.1)
{
    _liquid_error_downgrade_enable();
    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(8, 12);

    // check invalid function calls
    LIQUID_CHECK(NULL ==gasearch_create_advanced(                           NULL, NULL, prototype, LIQUID_OPTIM_MAXIMIZE, 16, 0.1f)) // bad utility function
    LIQUID_CHECK(NULL ==gasearch_create_advanced(gasearch_autotest_peak_callback, NULL,      NULL, LIQUID_OPTIM_MAXIMIZE,  0, 0.1f)) // bad parent chromosome
    LIQUID_CHECK(NULL ==gasearch_create_advanced(gasearch_autotest_peak_callback, NULL, prototype, LIQUID_OPTIM_MAXIMIZE,  0, 0.1f)) // bad population size
    LIQUID_CHECK(NULL ==gasearch_create_advanced(gasearch_autotest_peak_callback, NULL, prototype, LIQUID_OPTIM_MAXIMIZE, -1, 0.1f)) // bad population size
    LIQUID_CHECK(NULL ==gasearch_create_advanced(gasearch_autotest_peak_callback, NULL, prototype, LIQUID_OPTIM_MAXIMIZE, 16,-1.0f)) // bad mutation rate

    // create proper object and test configurations
    gasearch ga = gasearch_create(gasearch_autotest_peak_callback, NULL, prototype, LIQUID_OPTIM_MAXIMIZE);
    LIQUID_CHECK(LIQUID_OK == gasearch_print(ga))

    // test configurations
    LIQUID_CHECK(LIQUID_OK != gasearch_set_population_size(ga, 0, 8)) // population size too small
    LIQUID_CHECK(LIQUID_OK != gasearch_set_population_size(ga,-1, 8)) // population size too large
    LIQUID_CHECK(LIQUID_OK != gasearch_set_population_size(ga,24, 0)) // selection size too small
    LIQUID_CHECK(LIQUID_OK != gasearch_set_population_size(ga,24,24)) // selection size too large
    LIQUID_CHECK  (LIQUID_OK == gasearch_set_population_size(ga,24,12)) // ok
    LIQUID_CHECK(LIQUID_OK != gasearch_set_mutation_rate  (ga,-1.0f)) // mutation rate out of range
    LIQUID_CHECK(LIQUID_OK != gasearch_set_mutation_rate  (ga, 2.0f)) // mutation rate out of range
    LIQUID_CHECK  (LIQUID_OK == gasearch_set_mutation_rate  (ga, 0.1f)) // ok

    // destroy objects
    chromosome_destroy(prototype);
    gasearch_destroy(ga);
    _liquid_error_downgrade_disable();
}

// baseline tests using create_kaiser() method
LIQUID_AUTOTEST(chromosome, "baseline chromosome test", "", 0.1)
{
#if 0
    unsigned int bits_per_trait[] = {4, 8, 8, 4};
    chromosome p1 = chromosome_create(bits_per_trait, 4);
    chromosome p2 = chromosome_create(bits_per_trait, 4);
    chromosome c  = chromosome_create(bits_per_trait, 4);

    // 0000 11111111 00000000 1111
    p1->traits[0] = 0x0;
    p1->traits[1] = 0xFF;
    p1->traits[2] = 0x00;
    p1->traits[3] = 0xF;

    // 0101 01010101 01010101 0101
    p2->traits[0] = 0x5;
    p2->traits[1] = 0x55;
    p2->traits[2] = 0x55;
    p2->traits[3] = 0x5;

    printf("parent [1]:\n");
    chromosome_print(p1);

    printf("parent [2]:\n");
    chromosome_print(p2);

    printf("\n\n");

    // 
    // test crossover
    //

    printf("testing crossover...\n");

    chromosome_crossover(p1, p2, c, 0);
    // .... ........ ........ ....
    // 0101 01010101 01010101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 4);
    // 0000 ........ ........ ....
    // .... 01010101 01010101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 6);
    // 0000 11...... ........ ....
    // .... ..010101 01010101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 14);
    // 0000 11111111 00...... ....
    // .... ........ ..010101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 24);
    // 0000 11111111 00000000 1111
    // .... ........ ........ ....
    chromosome_print(c);

    //
    // test mutation
    //

    printf("testing mutation...\n");

    unsigned int i;
    for (i=0; i<24; i++) {
        chromosome_reset(c);
        chromosome_mutate(c,i);
        // 0000 01000000 00000000 0000
        chromosome_print(c);
    }

    chromosome_destroy(p1);
    chromosome_destroy(p2);
    chromosome_destroy(c);
#else
    LIQUID_PASS();
#endif
}

