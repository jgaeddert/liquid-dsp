/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Byte-packing utilities
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

void pack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int * num_written)
{
    div_t d = div(input_length,8);
    unsigned int req_output_length = d.quot;
    req_output_length += ( d.rem > 0 ) ? 1 : 0;
    if ( output_length < req_output_length ) {
        perror("error: pack_bytes(), output too short\n");
        exit(-1);
    }
    
    unsigned int i;
    unsigned int N = 0;         // number of bytes written to output
    unsigned char byte = 0;
    
    for (i=0; i<input_length; i++) {
        byte |= input[i] & 0x01;
        
        if ( (i+1)%8 == 0 ) {
            output[N++] = byte;
            byte = 0;
        } else {
            byte <<= 1;
        }
    }

    if ( i%8 != 0 )
        output[N++] = byte >> 1;
    
    *num_written = N;
}



void unpack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int * num_written)
{
    unsigned int i;
    unsigned int N = 0;
    unsigned char byte;

    if ( output_length < 8*input_length ) {
        perror("error: unpack_bytes(), output too short\n");
        exit(-1);
    }
    
    for (i=0; i<input_length; i++) {
        byte = input[i];
        output[N++] = (byte >> 7) & 0x01;
        output[N++] = (byte >> 6) & 0x01;
        output[N++] = (byte >> 5) & 0x01;
        output[N++] = (byte >> 4) & 0x01;
        output[N++] = (byte >> 3) & 0x01;
        output[N++] = (byte >> 2) & 0x01;
        output[N++] = (byte >> 1) & 0x01;
        output[N++] =  byte       & 0x01;
    }

    *num_written = N;
}

void repack_bytes(
    unsigned char * input,
    unsigned int input_sym_size,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_sym_size,
    unsigned int output_length,
    unsigned int * num_written)
{
    div_t d = div(input_length*input_sym_size,output_sym_size);
    unsigned int req_output_length = d.quot;
    req_output_length += ( d.rem > 0 ) ? 1 : 0;
    if ( output_length < req_output_length ) {
        perror("error: repack_bytes(), output too short\n");
        printf("  %u %u-bit symbols cannot be packed into %u %u-bit elements\n",
                input_length, input_sym_size,
                output_length, output_sym_size);
        exit(-1);
    }
    
    unsigned int i;
    unsigned char sym_in = 0;
    unsigned char sym_out = 0;

    // there is probably a more efficient way to do this, but...
    unsigned int total_bits = input_length*input_sym_size;
    unsigned int i_in = 0;  // input index
    unsigned int i_out = 0; // output index
    unsigned int k=0;       // input symbol enable
    unsigned int n=0;       // output symbol enable
    unsigned int v;         // bit mask

    for (i=0; i<total_bits; i++) {
        sym_out <<= 1;

        // push input if necessary
        if ( k == 0 ) {
            //printf("\n\ninput[%d] = %d", i_in, input[i_in]);
            sym_in = input[i_in++];
        }

        v = input_sym_size - k - 1;
        sym_out |= (sym_in >> v) & 0x01;
        //printf("\n    b = %d, v = %d, sym_in = %d, sym_out = %d", (sym_in >> v) & 0x01, v, sym_in, sym_out );

        // push output if available    
        if ( n == output_sym_size-1 ) {
            //printf("\n  output[%d] = %d", i_out, sym_out);
            output[i_out++] = sym_out;
            sym_out = 0;
        }
        k = (k+1) % input_sym_size;
        n = (n+1) % output_sym_size;

    }
    
    *num_written = i_out;
}

