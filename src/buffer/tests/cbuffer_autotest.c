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

//
// circular buffer autotest
//

#include <stdlib.h>
#include "autotest/autotest.h"
#include "liquid.h"

// floating point
void autotest_cbufferf()
{
    // input array of values
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};

    // output test arrays
    float test1[] = {1, 2, 3, 4};
    float test2[] = {3, 4, 1, 2, 3, 4, 5, 6, 7, 8};
    float test3[] = {3, 4, 5, 6, 7, 8};
    float test4[] = {3, 4, 5, 6, 7, 8, 1, 2, 3};
    float *r;                   // output read pointer
    unsigned int num_requested; // number of samples requested
    unsigned int num_read;      // number of samples read

    // create new circular buffer with 10 elements
    cbufferf q = cbufferf_create(10);
    // cbuffer: { <empty> }

    // part 1: write 4 elements to the buffer
    cbufferf_write(q, v, 4);
    // cbuffer: {1 2 3 4}

    // part 2: try to read 4 elements
    num_requested = 4;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float));

    // part 3: release two elements, write 8 more, read 10
    cbufferf_release(q, 2);
    // cbuffer: {3 4}
    cbufferf_write(q, v, 8);
    // cbuffer: {3 4 1 2 3 4 5 6 7 8}
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,10);
    CONTEND_SAME_DATA(r,test2,10*sizeof(float));

    // part 4: pop single element from buffer
    CONTEND_EQUALITY( cbufferf_size(q), 10    );
    cbufferf_pop(q, r);
    // cbuffer: {4 1 2 3 4 5 6 7 8}
    CONTEND_EQUALITY( cbufferf_size(q),  9    );
    CONTEND_EQUALITY( *r,                3.0f );

    // part 5: release three elements, and try reading 10
    cbufferf_release(q, 3);
    // cbuffer: {3 4 5 6 7 8}
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,6);
    CONTEND_SAME_DATA(r,test3,6*sizeof(float));

    // part 6: test pushing multiple elements
    cbufferf_push(q, 1);
    cbufferf_push(q, 2);
    cbufferf_push(q, 3);
    // cbuffer: {3 4 5 6 7 8 1 2 3}
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,9);
    CONTEND_SAME_DATA(r,test4,9*sizeof(float));

    // part 7: add one more element; buffer should be full
    CONTEND_EXPRESSION( cbufferf_is_full(q)==0 );
    cbufferf_push(q, 1);
    // cbuffer: {3 4 5 6 7 8 1 2 3 1}
    CONTEND_EXPRESSION( cbufferf_is_full(q)==1 );

    // memory leaks are evil
    cbufferf_destroy(q);
}


// complex float complexing point
void autotest_cbuffercf()
{
    // input array of values
    float complex v[] = {1.0 - 1.0 * _Complex_I,
                         2.0 + 2.0 * _Complex_I,
                         3.0 - 3.0 * _Complex_I,
                         4.0 + 4.0 * _Complex_I,
                         5.0 - 5.0 * _Complex_I,
                         6.0 + 6.0 * _Complex_I,
                         7.0 - 7.0 * _Complex_I,
                         8.0 + 8.0 * _Complex_I};

    // output test arrays
    float complex test1[] = {1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I};
    float complex test2[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I};
    float complex test3[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I};
    float complex test4[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I,
                             1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I};
    float complex *r;           // output read pointer
    unsigned int num_requested; // number of samples requested
    unsigned int num_read;      // number of samples read

    // create new circular buffer with 10 elements
    cbuffercf q = cbuffercf_create(10);
    // cbuffer: { <empty> }

    // part 1: write 4 elements to the buffer
    cbuffercf_write(q, v, 4);
    // cbuffer: {1 2 3 4}

    // part 2: try to read 4 elements
    num_requested = 4;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float complex));

    // part 3: release two elements, write 8 more, read 10
    cbuffercf_release(q, 2);
    // cbuffer: {3 4}
    cbuffercf_write(q, v, 8);
    // cbuffer: {3 4 1 2 3 4 5 6 7 8}
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,10);
    CONTEND_SAME_DATA(r,test2,10*sizeof(float complex));

    // part 4: pop single element from buffer
    CONTEND_EQUALITY( cbuffercf_size(q), 10    );
    cbuffercf_pop(q, r);
    // cbuffer: {4 1 2 3 4 5 6 7 8}
    CONTEND_EQUALITY( cbuffercf_size(q),  9    );
    CONTEND_EQUALITY( crealf(*r),         3.0f );
    CONTEND_EQUALITY( cimagf(*r),        -3.0f );

    // part 5: release three elements, and try reading 10
    cbuffercf_release(q, 3);
    // cbuffer: {3 4 5 6 7 8}
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,6);
    CONTEND_SAME_DATA(r,test3,6*sizeof(float complex));

    // part 6: test pushing multiple elements
    cbuffercf_push(q, 1.0 - 1.0 * _Complex_I);
    cbuffercf_push(q, 2.0 + 2.0 * _Complex_I);
    cbuffercf_push(q, 3.0 - 3.0 * _Complex_I);
    // cbuffer: {3 4 5 6 7 8 1 2 3}
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,9);
    CONTEND_SAME_DATA(r,test4,9*sizeof(float complex));

    // part 7: add one more element; buffer should be full
    CONTEND_EXPRESSION( cbuffercf_is_full(q)==0 );
    cbuffercf_push(q, 1.0 - 1.0 * _Complex_I);
    // cbuffer: {3 4 5 6 7 8 1 2 3 1}
    CONTEND_EXPRESSION( cbuffercf_is_full(q)==1 );

    // memory leaks are evil
    cbuffercf_destroy(q);
}

// test general flow
void autotest_cbufferf_flow()
{
    // options
    unsigned int max_size     =   48; // maximum number of elements in buffer
    unsigned int max_read     =   17; // maximum number of elements to read
    unsigned int num_elements = 1200; // total number of elements for run

    // flag to indicate if test was successful
    int success = 1;

    // temporary buffer to write samples before sending to cbuffer
    float write_buffer[max_size];

    // create new circular buffer
    cbufferf q = cbufferf_create_max(max_size, max_read);

    //
    unsigned i;
    unsigned write_id = 0;  // running total number of values written
    unsigned read_id  = 0;  // running total number of values read

    // continue running until
    while (1) {
        // write some values
        unsigned int num_available_to_write = cbufferf_space_available(q);

        // write samples if space is available
        if (num_available_to_write > 0) {
            // number of elements to write
            unsigned int num_to_write = (rand() % num_available_to_write) + 1;

            // generate samples to write
            for (i=0; i<num_to_write; i++) {
                write_buffer[i] = (float)(write_id);
                write_id++;
            }

            // write samples
            cbufferf_write(q, write_buffer, num_to_write);
        }

        // read some values
        unsigned int num_available_to_read = cbufferf_size(q);
        
        // read samples if available
        if (num_available_to_read > 0) {
            // number of elements to read
            unsigned int num_to_read = rand() % num_available_to_read;

            // read samples
            float *r;               // output read pointer
            unsigned int num_read;  // number of samples read
            cbufferf_read(q, num_to_read, &r, &num_read);

            // compare results
            for (i=0; i<num_read; i++) {
                if (liquid_autotest_verbose)
                    printf(" %s read %12.0f, expected %12u\n", r[i] == (float)read_id ? " " : "*", r[i], read_id);

                if (r[i] != (float)read_id)
                    success = 0;
                read_id++;
            }

            // release all the samples that were read
            cbufferf_release(q, num_read);
        }

        // stop on fail or upon completion
        if (!success || read_id >= num_elements)
            break;
    }
    
    // ensure test was successful
    CONTEND_EXPRESSION(success == 1);

    // destroy object
    cbufferf_destroy(q);
}


