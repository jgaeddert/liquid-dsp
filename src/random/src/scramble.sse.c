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
// Scramble (SSE SIMD version)
//

#if HAVE_SSE2
#include <emmintrin.h>
#endif

#if HAVE_SSE4_1
#include <smmintrin.h>
#endif

#include "liquid.internal.h"

void scramble_data(unsigned char * _x,
                   unsigned int _n)
{
    // t = 16*(floor(_n/16))
    unsigned int t = (_n >> 4) << 4;

    __m128i x;
    __m128i mask = _mm_set_epi8(LIQUID_SCRAMBLE_MASK3, LIQUID_SCRAMBLE_MASK2, LIQUID_SCRAMBLE_MASK1, LIQUID_SCRAMBLE_MASK0,
                                LIQUID_SCRAMBLE_MASK3, LIQUID_SCRAMBLE_MASK2, LIQUID_SCRAMBLE_MASK1, LIQUID_SCRAMBLE_MASK0,
                                LIQUID_SCRAMBLE_MASK3, LIQUID_SCRAMBLE_MASK2, LIQUID_SCRAMBLE_MASK1, LIQUID_SCRAMBLE_MASK0,
                                LIQUID_SCRAMBLE_MASK3, LIQUID_SCRAMBLE_MASK2, LIQUID_SCRAMBLE_MASK1, LIQUID_SCRAMBLE_MASK0);

    // apply static masks
    unsigned int i;
    for (i=0; i<t; i+=16) {
        x = _mm_lddqu_si128((const __m128i_u *)&_x[i]);

        x = _mm_xor_si128(x, mask);

        _mm_storeu_si128((__m128i_u *)&_x[i], x);
    }

    while(i < _n) {
        if (i < _n) _x[i++] ^= LIQUID_SCRAMBLE_MASK0;
        if (i < _n) _x[i++] ^= LIQUID_SCRAMBLE_MASK1;
        if (i < _n) _x[i++] ^= LIQUID_SCRAMBLE_MASK2;
        if (i < _n) _x[i++] ^= LIQUID_SCRAMBLE_MASK3;
    }
}

void unscramble_data(unsigned char * _x,
                     unsigned int _n)
{
    // for now apply simple static mask (re-run scramble)
    scramble_data(_x,_n);
}

// unscramble soft bits
//  _x      :   input message (soft bits) [size: 8*_n x 1]
//  _n      :   original message length (bytes)
void unscramble_data_soft(unsigned char * _x,
                          unsigned int _n)
{

#if HAVE_SSE4_1
    // t = 4*(floor(_n/4))
    unsigned int t = (_n >> 2) << 2;

    __m128i x;
    __m128i mask01 = _mm_set_epi8((LIQUID_SCRAMBLE_MASK1 & 0x01) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x02) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x04) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x08) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK1 & 0x10) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x20) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x40) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK1 & 0x80) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK0 & 0x01) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x02) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x04) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x08) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK0 & 0x10) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x20) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x40) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK0 & 0x80) ? 0xFF : 0);
    __m128i mask23 = _mm_set_epi8((LIQUID_SCRAMBLE_MASK3 & 0x01) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x02) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x04) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x08) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK3 & 0x10) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x20) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x40) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK3 & 0x80) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK2 & 0x01) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x02) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x04) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x08) ? 0xFF : 0,
                                  (LIQUID_SCRAMBLE_MASK2 & 0x10) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x20) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x40) ? 0xFF : 0, (LIQUID_SCRAMBLE_MASK2 & 0x80) ? 0xFF : 0);

    // apply static masks
    unsigned int i;
    for (i=0; i<t; i+=4) {
        x = _mm_lddqu_si128((const __m128i_u *)&_x[8 * i]);
        x = _mm_xor_si128(x, mask01);
        _mm_storeu_si128((__m128i_u *)&_x[8 * i], x);

        x = _mm_lddqu_si128((const __m128i_u *)&_x[8 * i + 16]);
        x = _mm_xor_si128(x, mask23);
        _mm_storeu_si128((__m128i_u *)&_x[8 * i + 16], x);
    }

    for (; i<_n; i++) {
#else
    // apply static masks
    unsigned int i;
    for (i=0; i<_n; i++) {
#endif
        unsigned char mask;

        switch ( i % 4 ) {
        case 0: mask = LIQUID_SCRAMBLE_MASK0; break;
        case 1: mask = LIQUID_SCRAMBLE_MASK1; break;
        case 2: mask = LIQUID_SCRAMBLE_MASK2; break;
        case 3: mask = LIQUID_SCRAMBLE_MASK3; break;
        default:;
        }

        if ( mask & 0x80 ) _x[8*i+0] ^= 255;
        if ( mask & 0x40 ) _x[8*i+1] ^= 255;
        if ( mask & 0x20 ) _x[8*i+2] ^= 255;
        if ( mask & 0x10 ) _x[8*i+3] ^= 255;
        if ( mask & 0x08 ) _x[8*i+4] ^= 255;
        if ( mask & 0x04 ) _x[8*i+5] ^= 255;
        if ( mask & 0x02 ) _x[8*i+6] ^= 255;
        if ( mask & 0x01 ) _x[8*i+7] ^= 255;
    }
}

