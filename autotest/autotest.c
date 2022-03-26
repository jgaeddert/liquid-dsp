/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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
// autotestlib.c
//

// default include headers
#include <stdio.h>
#include <stdlib.h>
#include "autotest/autotest.h"

// total number of checks invoked
unsigned long int liquid_autotest_num_checks=0;

// total number of checks which passed
unsigned long int liquid_autotest_num_passed=0;

// total number of checks which failed
unsigned long int liquid_autotest_num_failed=0;

// total number of warnings
unsigned long int liquid_autotest_num_warnings=0;

// verbosity flag
int liquid_autotest_verbose = 1;

// fail test
// increment liquid_autotest_num_checks
// increment liquid_autotest_num_failed
void liquid_autotest_failed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_failed++;
}

// pass test
// increment liquid_autotest_num_checks
// increment liquid_autotest_num_passed
void liquid_autotest_passed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_passed++;
}

// fail test, given expression
//  _file       :   filename (string)
//  _line       :   line number of test
//  _exprL      :   left side of expression (string)
//  _valueL     :   left side of expression (value)
//  _qualifier  :   expression qualifier
//  _exprR      :   right side of expression (string)
//  _valueR     :   right side of expression (value)
void liquid_autotest_failed_expr(const char * _file,
                                 unsigned int _line,
                                 const char * _exprL,
                                 double _valueL,
                                 const char * _qualifier,
                                 const char * _exprR,
                                 double _valueR)
{
    if (liquid_autotest_verbose) {
        printf("  TEST FAILED: %s line %u : expected %s (%0.2E) %s %s (%0.2E)\n",
                _file, _line, _exprL, _valueL, _qualifier, _exprR, _valueR);
    }
    liquid_autotest_failed();
}

// fail test, given true/false value
//  _file       :   filename (string)
//  _line       :   line number of test
//  _exprL      :   left side of expression (string)
//  _valueL     :   left side of expression (value)
//  _qualifier  :   expression qualifier
void liquid_autotest_failed_bool(const char * _file,
                                 unsigned int _line,
                                 const char * _exprL,
                                 double       _valueL,
                                 int          _qualifier)
{
    if (liquid_autotest_verbose) {
        printf("  TEST FAILED: %s line %u : expected \"%s\" (%g) is %s\n",
                _file, _line, _exprL, _valueL, _qualifier ? "true" : "false");
    }
    liquid_autotest_failed();
}

//  _file       :   filename (string)
//  _line       :   line number of test
//  _message    :   message string
void liquid_autotest_failed_msg(const char * _file,
                                unsigned int _line,
                                const char * _message)
{
    if (liquid_autotest_verbose)
        printf("  TEST FAILED: %s line %u : %s\n", _file, _line, _message);
    liquid_autotest_failed();
}

// print basic autotest results to stdout
void autotest_print_results(void)
{
    if (liquid_autotest_num_warnings > 0) {
        printf("==================================\n");
        printf(" WARNINGS : %-lu\n", liquid_autotest_num_warnings);
    }

    printf("==================================\n");
    if (liquid_autotest_num_checks==0) {
        printf(" NO CHECKS RUN\n");
    } else if (liquid_autotest_num_failed==0) {
        printf(" PASSED ALL %lu CHECKS\n", liquid_autotest_num_passed);
    } else {
        // compute and print percentage of failed tests
        double percent_failed = (double) liquid_autotest_num_failed /
                                (double) liquid_autotest_num_checks;
        printf(" FAILED %lu / %lu CHECKS (%7.2f%%)\n",
                liquid_autotest_num_failed,
                liquid_autotest_num_checks,
                100.0*percent_failed);
    }
    printf("==================================\n");
}

// print warning to stderr
// increment liquid_autotest_num_warnings
//  _file       :   filename (string)
//  _line       :   line number of test
//  _message    :   message string
void liquid_autotest_warn(const char * _file,
                          unsigned int _line,
                         const char * _message)
{
    if (liquid_autotest_verbose)
        fprintf(stderr,"  WARNING: %s line %u : %s\n", _file, _line, _message);

    liquid_autotest_num_warnings++;
}

// contend that data in two arrays are identical
//  _x      :   input array [size: _n x 1]
//  _y      :   input array [size: _n x 1]
//  _n      :   input array size
int liquid_autotest_same_data(unsigned char * _x,
                              unsigned char * _y,
                              unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_x[i] != _y[i])
            return 0;
    }
    return 1;
}

// print array to standard out
//  _x      :   input array [size: _n x 1]
//  _n      :   input array size
void liquid_autotest_print_array(unsigned char * _x,
                                 unsigned int _n)
{
    unsigned int i;
    printf("   {");
    for (i=0; i<_n; i++) {
        printf("%.2x, ", (unsigned int)(_x[i]));
        if ( ((i+1)%16 == 0) && (i != (_n-1)) )
            printf("\n    ");
    }
    printf("}\n");
}

// validate spectral content
int liquid_autotest_validate_spectrum(float * _psd, unsigned int _nfft,
        autotest_psd_s * _regions, unsigned int _num_regions, const char * _debug_filename)
{
    unsigned int i;
#if 0
    //for (i=0; i<_num_regions; i++)...
    // determine appropriate indices
    unsigned int i0 = ((unsigned int)roundf((_fc+0.5f)*_nfft)) % _nfft;
    unsigned int ns = (unsigned int)roundf(_nfft*(1.0f-beta)/(float)k); // numer of samples to observe
    float psd_target = 10*log10f(powf(10.0f,noise_floor/10.0f) + powf(10.0f,(noise_floor+_SNRdB)/10.0f));
    //printf("i0=%u, ns=%u (nfft=%u), target=%.3f dB\n", i0, ns, _nfft, psd_target);

    // verify result
    float psd[_nfft];
    spgramcf_get_psd(q, psd);
    // debug
    const char filename[] = "testbench_spgramcf_signal.m";
    for (i=0; i<ns; i++) {
        unsigned int index = (i0 + i + _nfft - ns/2) % _nfft;
        CONTEND_DELTA(psd[index], psd_target, tol)
    }
#endif

    // export debug file if requested
    if (_debug_filename != NULL) {
        FILE * fid = fopen(_debug_filename,"w");
        if (fid == NULL) {
            fprintf(stderr,"could not open '%s' for writing\n", _debug_filename);
            return -1;
        }
        fprintf(fid,"clear all; close all; nfft=%u; f=[0:(nfft-1)]/nfft-0.5; psd=zeros(1,nfft);\n", _nfft);
        for (i=0; i<_nfft; i++) { fprintf(fid,"psd(%6u) = %8.2f;\n", i+1, _psd[i]); }
        fprintf(fid,"figure; xlabel('f/F_s'); ylabel('PSD [dB]'); hold on;\n");
        // add target regions
        for (i=0; i<_num_regions; i++) {
            fprintf(fid,"  plot([%f,%f],[%f,%f],'Color',[0.5 0 0]);\n",_regions[i].fmin,_regions[i].fmax,_regions[i].pmin,_regions[i].pmin);
            fprintf(fid,"  plot([%f,%f],[%f,%f],'Color',[0.5 0 0]);\n",_regions[i].fmin,_regions[i].fmax,_regions[i].pmax,_regions[i].pmax);
        }
        // plot spectrum
        fprintf(fid,"  plot(f,psd,'LineWidth',2,'Color',[0 0.3 0.5]);\n");
        fprintf(fid,"hold off; grid on; xlim([-0.5 0.5]);\n");
        fclose(fid);
        printf("debug file written to %s\n", _debug_filename);
    }
    return 0;
}

/*
int liquid_autotest_validate_spectrum2(spgramcf _periodogram, autotest_psd_s * _regions)
{
    return 0;
}

int liquid_autotest_validate_spectrum3(firfilt_crcf _filter, autotest_psd_s * _regions)
{
    return 0;
}

int liquid_autotest_validate_spectrum4(iirfilt_crcf _filter, autotest_psd_s * _regions)
{
    return 0;
}
*/

