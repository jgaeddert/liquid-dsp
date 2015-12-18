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
// pack_bytes.c
//
// Useful byte-packing utilities
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// pack binary array with a symbol
//  _dest       :   destination array [size: _n x 1]
//  _n          :   output source array length
//  _k          :   bit index to write in _dest
//  _b          :   number of bits in input symbol
//  _sym_in     :   input symbol
void liquid_pack_array(unsigned char * _dest,
                       unsigned int _n,
                       unsigned int _k,
                       unsigned int _b,
                       unsigned int _sym_in)
{
    // validate input
    if (_k >= 8*_n) {
        fprintf(stderr,"error: liquid_pack_array(), bit index exceeds array length\n");
        exit(1);
    }

    // find base index
    unsigned int i0 = _k / 8;       // byte index
    unsigned int b0 = _k - 8*i0;    // bit index
    //printf("base index : %2u, %2u\n", i0, b0);

    // take chunks of bits from _sym_in until it is depleted
    // we'll first fill what's left of (8 - b0) and then go one byte at a time
    while (_b > 0 && i0 < _n) {
        unsigned int n = 8 - b0;
        // clamp at the smaller of (8 - b0, _b)
        if (_b < n) {
            n = _b;
        }

        // build a right-justified 1s mask
        unsigned char mask = 0xff >> (8-n);
        // mask out n bits, 1 <= n <= 8. shift right to get the top bits
        // if _b > (8 - b0) then we take the top (8 - b0) bits of sym
        // if _b <= (8 - b0) then we take all the bits
        unsigned char sym = (_sym_in >> (_b - n)) & mask;

        // now move the mask and the masked bits left to accomodate b0
        mask <<= (8-n-b0);
        sym <<= (8-n-b0);

        // mask in place
        _dest[i0] &= ~mask;
        _dest[i0] |= sym;

#if 0
        printf("  output symbol byte iteration\n");
        printf("  n    : %u\n", n);
        printf("  _b   : %u\n" _b);
        printf("  mask : 0x%.2x\n", mask);
        printf("  sym  : 0x%.2x\n", sym);
#endif

        b0 += n;
        if (b0 >= 8) {
            b0 %= 8;
            i0++;
        }
        _b -= n;
    }
}

// pack binary array with block of equally-sized symbols
//  _dest       :   destination array [size: _n x 1]
//  _n          :   output source array length
//  _b          :   number of bits in input symbol
//  _m          :   input symbol array length
//  _syms_in    :   input symbol array [size: _m x 1]
void liquid_pack_array_block(unsigned char * _dest,
                             unsigned int _n,
                             unsigned int _b,
                             unsigned int _m,
                             unsigned int * _syms_in)
{
    unsigned int k = 0;
    unsigned int i;
    for (i = 0; i < _m; i++, k += _b) {
        liquid_pack_array(_dest, _n, k, _b, _syms_in[i]);
    }
}


// unpack symbols from binary array
//  _src        :   source array [size: _n x 1]
//  _n          :   input source array length
//  _k          :   bit index to read from _src
//  _b          :   number of bits in output symbol
//  _sym_out    :   output symbol
void liquid_unpack_array(unsigned char * _src,
                         unsigned int _n,
                         unsigned int _k,
                         unsigned int _b,
                         unsigned int * _sym_out)
{
    // validate input
    if (_k >= 8*_n) {
        fprintf(stderr,"error: liquid_unpack_array(), bit index exceeds array length\n");
        exit(1);
    }

    // find base index
    unsigned int i0 = _k / 8;       // byte index
    unsigned int b0 = _k - 8*i0;    // bit index
    //printf("base index : %2u, %2u\n", i0, b0);

    unsigned int out = 0;

    // take chunks of bits from _src until _sym_out is big enough
    // we'll first fill what's left of (8 - b0) and then go one byte at a time
    while (_b > 0 && i0 < _n) {
        unsigned int n = 8 - b0;
        // clamp at the smaller of (8 - b0, _b)
        if (_b < n) {
            n = _b;
        }

        // build a right-justified 1s mask
        unsigned char mask = 0xff >> (8-n);
        // mask out n bits, 1 <= n <= 8. shift right to get the top bits
        // if _b >= (8 - b0) then we don't shift at all (use all bits)
        // if _b < (8 - b0) then we shift by the number of bits we have but aren't using
        unsigned char sym = (_src[i0] >> ((8 - b0) - n)) & mask;

        out <<= n;
        out |= sym;
#if 0
        printf("  output symbol iteration\n");
        printf("  n    : %u\n", n);
        printf("  _b   : %u\n" _b);
        printf("  mask : 0x%.2x\n", mask);
        printf("  sym  : 0x%.2x\n", sym);
#endif

        b0 += n;
        if (b0 >= 8) {
            b0 %= 8;
            i0++;
        }
        _b -= n;

    }
    out <<= _b;
    *_sym_out = out;
}

// unpack symbols from binary array
//  _src        :   source array [size: _n x 1]
//  _n          :   input source array length
//  _b          :   number of bits in output symbol
//  _m          :   output symbol array length
//  _syms_out   :   output symbol array [size: _m x 1]
void liquid_unpack_array_block(unsigned char * _src,
                               unsigned int _n,
                               unsigned int _b,
                               unsigned int _m,
                               unsigned int * _syms_out)
{
    unsigned int i, k;
    for (i = 0, k = 0; i < _m && k < 8*_n; i++, k += _b) {
        liquid_unpack_array(_src, _n, k, _b, &_syms_out[i]);
    }
}


// pack one-bit symbols into bytes (8-bit symbols)
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_pack_bytes(unsigned char * _sym_in,
                       unsigned int _sym_in_len,
                       unsigned char * _sym_out,
                       unsigned int _sym_out_len,
                       unsigned int * _num_written)
{
    div_t d = div(_sym_in_len,8);
    unsigned int req__sym_out_len = d.quot;
    req__sym_out_len += ( d.rem > 0 ) ? 1 : 0;
    if ( _sym_out_len < req__sym_out_len ) {
        fprintf(stderr,"error: pack_bytes(), output too short\n");
        exit(-1);
    }
    
    unsigned int i;
    unsigned int N = 0;         // number of bytes written to output
    unsigned char byte = 0;
    
    for (i=0; i<_sym_in_len; i++) {
        byte |= _sym_in[i] & 0x01;
        
        if ( (i+1)%8 == 0 ) {
            _sym_out[N++] = byte;
            byte = 0;
        } else {
            byte <<= 1;
        }
    }

    if ( i%8 != 0 )
        _sym_out[N++] = byte >> 1;
    
    *_num_written = N;
}



// unpack 8-bit symbols (full bytes) into one-bit symbols
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols array
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_unpack_bytes(unsigned char * _sym_in,
                         unsigned int _sym_in_len,
                         unsigned char * _sym_out,
                         unsigned int _sym_out_len,
                         unsigned int * _num_written)
{
    unsigned int i;
    unsigned int n = 0;
    unsigned char byte;

    if ( _sym_out_len < 8*_sym_in_len ) {
        fprintf(stderr,"error: unpack_bytes(), output too short\n");
        exit(-1);
    }
    
    for (i=0; i<_sym_in_len; i++) {
        // read input byte
        byte = _sym_in[i];

        // unpack byte into 8 one-bit symbols
        _sym_out[n++] = (byte >> 7) & 0x01;
        _sym_out[n++] = (byte >> 6) & 0x01;
        _sym_out[n++] = (byte >> 5) & 0x01;
        _sym_out[n++] = (byte >> 4) & 0x01;
        _sym_out[n++] = (byte >> 3) & 0x01;
        _sym_out[n++] = (byte >> 2) & 0x01;
        _sym_out[n++] = (byte >> 1) & 0x01;
        _sym_out[n++] =  byte       & 0x01;
    }

    *_num_written = n;
}

// repack bytes with arbitrary symbol sizes
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_bps         :   number of bits per input symbol
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols array
//  _sym_out_bps        :   number of bits per output symbol
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_repack_bytes(unsigned char * _sym_in,
                         unsigned int _sym_in_bps,
                         unsigned int _sym_in_len,
                         unsigned char * _sym_out,
                         unsigned int _sym_out_bps,
                         unsigned int _sym_out_len,
                         unsigned int * _num_written)
{
    // compute number of output symbols and determine if output array
    // is sufficiently sized
    div_t d = div(_sym_in_len*_sym_in_bps,_sym_out_bps);
    unsigned int req__sym_out_len = d.quot;
    req__sym_out_len += ( d.rem > 0 ) ? 1 : 0;
    if ( _sym_out_len < req__sym_out_len ) {
        fprintf(stderr,"error: repack_bytes(), output too short\n");
        fprintf(stderr,"  %u %u-bit symbols cannot be packed into %u %u-bit elements\n",
                _sym_in_len, _sym_in_bps,
                _sym_out_len, _sym_out_bps);
        exit(-1);
    }
    
    unsigned int i;
    unsigned char s_in = 0;     // input symbol
    unsigned char s_out = 0;    // output symbol

    // there is probably a more efficient way to do this, but...
    unsigned int total_bits = _sym_in_len*_sym_in_bps;
    unsigned int i_in = 0;  // input index counter
    unsigned int i_out = 0; // output index counter
    unsigned int k=0;       // input symbol enable
    unsigned int n=0;       // output symbol enable
    unsigned int v;         // bit mask

    for (i=0; i<total_bits; i++) {
        // shift output symbol by one bit
        s_out <<= 1;

        // pop input if necessary
        if ( k == 0 ) {
            //printf("\n\n_sym_in[%d] = %d", i_in, _sym_in[i_in]);
            s_in = _sym_in[i_in++];
        }

        // compute shift amount and append input bit at index
        // to output symbol
        v = _sym_in_bps - k - 1;
        s_out |= (s_in >> v) & 0x01;
        //printf("\n    b = %d, v = %d, s_in = %d, sym_out = %d", (sym_in >> v) & 0x01, v, s_in, sym_out );

        // push output if available    
        if ( n == _sym_out_bps-1 ) {
            //printf("\n  _sym_out[%d] = %d", i_out, sym_out);
            _sym_out[i_out++] = s_out;
            s_out = 0;
        }

        // update input/output symbol pop/push flags
        k = (k+1) % _sym_in_bps;
        n = (n+1) % _sym_out_bps;
    }

    // if uneven, push zeros into remaining output symbol
    if (i_out != req__sym_out_len) {
        for (i=n; i<_sym_out_bps; i++)
            s_out <<= 1;
        _sym_out[i_out++] = s_out;
    }
    
    *_num_written = i_out;
}

