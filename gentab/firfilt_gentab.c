/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

// generate a small set of pre-computed FIR filter coefficients

#include <stdio.h>
#include <stdlib.h>

#include <liquid/liquid.h>

// print weights as C-style array
int export_weights(const float * _h,
                   unsigned int  _n,
                   const char *  _varname,
                   FILE *        _fid);

// design halfband filter and export to file
int design_halfband(unsigned int _m, float _ft, const char * _varname, FILE * _fid);

//
int main(int argc, char*argv[])
{
    FILE * fid = stdout;

    fprintf(fid,"\n");
    fprintf(fid,"// half-band filter: m = 12, As = 80 dB\n");
    design_halfband(12, 0.1, "liquid_firfilt_m12_M2_a80", fid);

    return 0;
}

// print weights as C-style array
int export_weights(const float * _h,
                   unsigned int  _n,
                   const char *  _varname,
                   FILE *        _fid)
{
    fprintf(_fid, "const float %s[%u] = {\n", _varname, _n);
    fprintf(_fid, "  ");
    unsigned int i;
    for (i=0; i<_n; i++) {
        printf("%20.12e,", _h[i]);

        if ( i != _n-1 ) {
            if ( ((i+1)%2)==0 )
                printf("\n  ");
            else
                printf(" ");
        }

    }
    printf("};\n\n");
    return LIQUID_OK;
}

// design halfband filter and export to file
int design_halfband(unsigned int _m,
                    float        _ft,
                    const char * _varname,
                    FILE *       _fid)
{
    // design filter
    unsigned int h_len = 4*_m + 1;
    float h[h_len];
    liquid_firdespm_halfband_ft(_m, _ft, h);

    // TODO: scale coefficients by 2 to have peak value at 1?

    // export coefficients
    return export_weights(h, h_len, _varname, _fid);
}

