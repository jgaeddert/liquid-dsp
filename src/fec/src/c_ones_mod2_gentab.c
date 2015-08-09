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
// script to generate c_ones_mod2 table
//

#include <stdio.h>

// count the number of ones in a byte
unsigned int count_ones(unsigned char);

int main() {

    unsigned char c_ones_mod2[256];

    unsigned int b;
    for (b=0; b<256; b++)
        c_ones_mod2[b] = count_ones((unsigned char)b) % 2;
    
    // print results
    printf("unsigned char c_ones_mod2[] = {\n    ");
    for (b=0; b<256; b++) {
        printf("%1u", c_ones_mod2[b]);
        if (b==255)
            printf("\n");
        else if (((b+1)%16)==0)
            printf(",\n    ");
        else
            printf(", ");
    }
    printf("};\n");
    return 0;
}

// count the number of ones in a byte
unsigned int count_ones(unsigned char _b)
{
    unsigned int i, n=0;
    for (i=0; i<8; i++) {
        n += _b & 0x01;
        _b >>= 1;
    }
    return n;
}
