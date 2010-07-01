/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// pack_bytes.c
//
// Useful byte-packing utilities
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// pack one-bit symbols into bytes (8-bit symbols)
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void pack_bytes(unsigned char * _sym_in,
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
void unpack_bytes(unsigned char * _sym_in,
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
void repack_bytes(unsigned char * _sym_in,
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

