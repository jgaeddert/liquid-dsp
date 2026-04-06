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

#include <stdlib.h>
#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(pack_array,"pack array","",0.1) {
    // input symbols
    unsigned int sym_size[9] = {8, 2, 3, 6, 1, 3, 3, 4, 3};
    unsigned char input[9] = {
        0x81,   // 1000 0001
        0x03,   //        11
        0x05,   //       101
        0x3a,   //   11 1010
        0x01,   //         1
        0x07,   //       111
        0x06,   //       110
        0x0a,   //      1010
        0x04    //     10[0] <- last bit is stripped
    };

    // output       : 1000 0001 1110 1111 0101 1111 1010 1010
    // symbol       : 0000 0000 1122 2333 3334 5556 6677 7788
    unsigned char output_test[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output[4]      = {0xff, 0xff, 0xff, 0xff};

    unsigned int k=0;
    unsigned int i;
    for (i=0; i<9; i++) {
        liquid_pack_array(output, 4, k, sym_size[i], input[i]);
        k += sym_size[i];
    }
    
    LIQUID_CHECK_ARRAY( output, output_test, 4 );
}

LIQUID_AUTOTEST(unpack_array,"unpack array","",0.1) {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010
    // symbol       : 0000 0000 1122 2333 3334 5556 6677 7788
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned int sym_size[9] = {8, 2, 3, 6, 1, 3, 3, 4, 3};

    // output syms
    unsigned char output_test[9] = {
        0x81,   // 1000 0001
        0x03,   //        11
        0x05,   //       101
        0x3a,   //   11 1010
        0x01,   //         1
        0x07,   //       111
        0x06,   //       110
        0x0a,   //      1010
        0x04    //     10[0] <- last bit is implied
    };

    unsigned char output[9];

    unsigned int k=0;
    unsigned int i;
    for (i=0; i<9; i++) {
        liquid_unpack_array(input, 4, k, sym_size[i], &output[i]);
        k += sym_size[i];
    }
    
    LIQUID_CHECK_ARRAY( output, output_test, 9 );
}

LIQUID_AUTOTEST(repack_array,"unpack/pack array","",0.1) {
    unsigned int n=512;     // input/output array size
    unsigned char src[n];   // original data array
    unsigned char dst[n];   // repacked data array

    unsigned int i;
    // initialize input/output arrays
    for (i=0; i<n; i++) {
        src[i] = rand() & 0xff;
        dst[i] = 0x00;
    }

    unsigned int k=0;
    unsigned char sym=0;
    unsigned int sym_size=0;
    while (k < 8*n) {
        // random symbol size
        sym_size = (rand()%8) + 1;

        // unpack symbol from input array
        liquid_unpack_array(src, n, k, sym_size, &sym);

        // pack symbol into output array
        liquid_pack_array(dst, n, k, sym_size, sym);

        // update bit index counter
        k += sym_size;
    }
    
    LIQUID_CHECK_ARRAY( src, dst, n );
}

LIQUID_AUTOTEST(pack_bytes_01,"pack bytes","",0.1) {
    unsigned char output[8];
    unsigned int N;
    
    unsigned char input[36] = {
        0, 0, 0, 0, 0, 0, 0, 0, // 0:   0000 0000
        1, 1, 1, 1, 1, 1, 1, 1, // 255: 1111 1111
        0, 0, 0, 0, 1, 1, 1, 1, // 15:  0000 1111
        1, 0, 1, 0, 1, 0, 1, 0  // 170: 1010 1010
    };

    // Test packing entire array
    unsigned char output_test_01[4] = {0x00, 0xFF, 0x0F, 0xAA};
    liquid_pack_bytes( input, 32, output, 8, &N );
    LIQUID_CHECK( N ==  4 );
    LIQUID_CHECK_ARRAY( output, output_test_01, 4 );

    // Test packing only 28 elements
    unsigned char output_test_02[4] = {0x00, 0xFF, 0x0F, 0x0A};
    liquid_pack_bytes( input, 28, output, 8, &N );
    LIQUID_CHECK( N ==  4 );
    LIQUID_CHECK_ARRAY( output, output_test_02, 4 );
    
    // Test packing only 25 elements
    unsigned char output_test_03[4] = {0x00, 0xFF, 0x0F, 0x01};
    liquid_pack_bytes( input, 25, output, 8, &N );
    LIQUID_CHECK( N ==  4 );
    LIQUID_CHECK_ARRAY( output, output_test_03, 4 );

    // Test packing only 24 elements (3 bytes)
    unsigned char output_test_04[3] = {0x00, 0xFF, 0x0F};
    liquid_pack_bytes( input, 24, output, 8, &N );
    LIQUID_CHECK( N ==  3 );
    LIQUID_CHECK_ARRAY( output, output_test_04, 3 );
}


LIQUID_AUTOTEST(unpack_bytes_01,"unpack bytes","",0.1) {
    unsigned char input[5] = {0x00, 0x01, 0xFF, 0x0F, 0xAA};
    
    unsigned char output[64];
    unsigned int N;

    unsigned char output_test[40] = {
        0, 0, 0, 0, 0, 0, 0, 0, // 0:   0000 0000
        0, 0, 0, 0, 0, 0, 0, 1, // 1:   0000 0001
        1, 1, 1, 1, 1, 1, 1, 1, // 255: 1111 1111
        0, 0, 0, 0, 1, 1, 1, 1, // 15:  0000 1111
        1, 0, 1, 0, 1, 0, 1, 0  // 170: 1010 1010
    };
    
    // Test packing entire array
    liquid_unpack_bytes( input, 4, output, 40, &N );
    LIQUID_CHECK( N ==  32 );
    LIQUID_CHECK_ARRAY( output, output_test, 32 );
}


LIQUID_AUTOTEST(repack_bytes_01,"repack bytes","",0.1) {
    unsigned char input[] = {
        0x07,   // 111
        0x00,   // 000
        0x06,   // 110
        0x07    // 111
    };

    unsigned char output_test[] = {
        0x03,   // 11
        0x02,   // 10
        0x00,   // 00
        0x03,   // 11
        0x01,   // 01
        0x03    // 11
    };

    unsigned char output[6];
    unsigned int N;

    liquid_repack_bytes( input, 3, 4, output, 2, 6, &N );

    LIQUID_CHECK( N ==  6 );
    LIQUID_CHECK_ARRAY( output, output_test, 6 );
}

LIQUID_AUTOTEST(repack_bytes_02,"repack bytes","",0.1) {
    unsigned char input[] = {
        0x01,   // 00001
        0x02,   // 00010
        0x04    // 00100
    };

    unsigned char output_test[] = {
        0x00,   // 000
        0x02,   // 010
        0x01,   // 001
        0x00,   // 000
        0x04    // 100
    };

    unsigned char output[5];
    unsigned int N;

    liquid_repack_bytes( input, 5, 3, output, 3, 5, &N );

    LIQUID_CHECK( N ==  5 );
    LIQUID_CHECK_ARRAY( output, output_test, 5 );
}

LIQUID_AUTOTEST(repack_bytes_03,"repack bytes","",0.1) {
    unsigned char input[] = {
        0x00,   // 000
        0x02,   // 010
        0x01,   // 001
        0x00,   // 000
        0x04    // 100
    };

    unsigned char output_test[] = {
        0x01,   // 00001
        0x02,   // 00010
        0x04    // 00100
    };

    unsigned char output[3];
    unsigned int N;

    liquid_repack_bytes( input, 3, 5, output, 5, 3, &N );

    LIQUID_CHECK( N ==  3 );
    LIQUID_CHECK_ARRAY( output, output_test, 3 );
}

LIQUID_AUTOTEST(repack_bytes_04_uneven,"repack bytes","",0.1) {
    unsigned char input[3] = {
        0x07,   // 111
        0x07,   // 111
        0x07    // 111(0)
    };

    unsigned char output_test[] = {
        0x03,   // 11
        0x03,   // 11
        0x03,   // 11
        0x03,   // 11
        0x02    // 10
    };

    unsigned char output[5];
    unsigned int N;

    liquid_repack_bytes( input, 3, 3, output, 2, 5, &N );

    LIQUID_CHECK( N ==  5 );
    LIQUID_CHECK_ARRAY( output, output_test, 5 );
}

