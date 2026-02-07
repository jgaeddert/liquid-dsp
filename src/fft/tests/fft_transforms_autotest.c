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

#include "liquid.h"
#include "liquid.autotest.h"

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void testbench_fft(liquid_autotest __q__,
                   float complex * _x,
                   float complex * _test,
                   unsigned int    _n)
{
    int _method = 0;
    float tol=2e-4f;

    unsigned int i;

    float complex y[_n], z[_n];

    // compute FFT
    fftplan pf = fft_create_plan(_n, _x, y, LIQUID_FFT_FORWARD, _method);
    fft_execute(pf);

    // compute IFFT
    fftplan pr = fft_create_plan(_n, y, z, LIQUID_FFT_BACKWARD, _method);
    fft_execute(pr);

    // normalize inverse
    for (i=0; i<_n; i++)
        z[i] /= (float) _n;

    // validate results
    float fft_error, ifft_error;
    for (i=0; i<_n; i++) {
        fft_error = cabsf( y[i] - _test[i] );
        ifft_error = cabsf( _x[i] - z[i] );
        LIQUID_CHECK_DELTA( fft_error,  0, tol);
        LIQUID_CHECK_DELTA( ifft_error, 0, tol);
    }

    // destroy plans
    fft_destroy_plan(pf);
    fft_destroy_plan(pr);
}


// 
// AUTOTESTS: small FFTs
//
extern float complex fft_test_x3[];
extern float complex fft_test_y3[];

extern float complex fft_test_x5[];
extern float complex fft_test_y5[];

extern float complex fft_test_x6[];
extern float complex fft_test_y6[];

extern float complex fft_test_x7[];
extern float complex fft_test_y7[];

extern float complex fft_test_x9[];
extern float complex fft_test_y9[];

LIQUID_AUTOTEST(fft_3,"small transforms","fft",0.1) { testbench_fft(__q__, fft_test_x3, fft_test_y3, 3); }
LIQUID_AUTOTEST(fft_5,"small transforms","fft",0.1) { testbench_fft(__q__, fft_test_x5, fft_test_y5, 5); }
LIQUID_AUTOTEST(fft_6,"small transforms","fft",0.1) { testbench_fft(__q__, fft_test_x6, fft_test_y6, 6); }
LIQUID_AUTOTEST(fft_7,"small transforms","fft",0.1) { testbench_fft(__q__, fft_test_x7, fft_test_y7, 7); }
LIQUID_AUTOTEST(fft_9,"small transforms","fft",0.1) { testbench_fft(__q__, fft_test_x9, fft_test_y9, 9); }


// 
// AUTOTESTS: power-of-two transforms
//

extern float complex fft_test_x2[];
extern float complex fft_test_y2[];

extern float complex fft_test_x4[];
extern float complex fft_test_y4[];

extern float complex fft_test_x8[];
extern float complex fft_test_y8[];

extern float complex fft_test_x16[];
extern float complex fft_test_y16[];

extern float complex fft_test_x32[];
extern float complex fft_test_y32[];

extern float complex fft_test_x64[];
extern float complex fft_test_y64[];

LIQUID_AUTOTEST(fft_2, "radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x2,   fft_test_y2,      2);     }
LIQUID_AUTOTEST(fft_4, "radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x4,   fft_test_y4,      4);     }
LIQUID_AUTOTEST(fft_8, "radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x8,   fft_test_y8,      8);     }
LIQUID_AUTOTEST(fft_16,"radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x16,  fft_test_y16,     16);    }
LIQUID_AUTOTEST(fft_32,"radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x32,  fft_test_y32,     32);    }
LIQUID_AUTOTEST(fft_64,"radix-2 transforms","fft",0.1) { testbench_fft(__q__, fft_test_x64,  fft_test_y64,     64);    }

// 
// AUTOTESTS: composite transforms
//
//
extern float complex fft_test_x10[];
extern float complex fft_test_y10[];

extern float complex fft_test_x20[];
extern float complex fft_test_y20[];

extern float complex fft_test_x21[];
extern float complex fft_test_y21[];

extern float complex fft_test_x22[];
extern float complex fft_test_y22[];

extern float complex fft_test_x24[];
extern float complex fft_test_y24[];

extern float complex fft_test_x26[];
extern float complex fft_test_y26[];

extern float complex fft_test_x30[];
extern float complex fft_test_y30[];

extern float complex fft_test_x35[];
extern float complex fft_test_y35[];

extern float complex fft_test_x36[];
extern float complex fft_test_y36[];

extern float complex fft_test_x48[];
extern float complex fft_test_y48[];

extern float complex fft_test_x63[];
extern float complex fft_test_y63[];

extern float complex fft_test_x92[];
extern float complex fft_test_y92[];

extern float complex fft_test_x96[];
extern float complex fft_test_y96[];

extern float complex fft_test_x120[];
extern float complex fft_test_y120[];

extern float complex fft_test_x130[];
extern float complex fft_test_y130[];

extern float complex fft_test_x192[];
extern float complex fft_test_y192[];

LIQUID_AUTOTEST(fft_10, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x10,   fft_test_y10,   10); }
LIQUID_AUTOTEST(fft_21, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x21,   fft_test_y21,   21); }
LIQUID_AUTOTEST(fft_22, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x22,   fft_test_y22,   22); }
LIQUID_AUTOTEST(fft_24, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x24,   fft_test_y24,   24); }
LIQUID_AUTOTEST(fft_26, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x26,   fft_test_y26,   26); }
LIQUID_AUTOTEST(fft_30, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x30,   fft_test_y30,   30); }
LIQUID_AUTOTEST(fft_35, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x35,   fft_test_y35,   35); }
LIQUID_AUTOTEST(fft_36, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x36,   fft_test_y36,   36); }
LIQUID_AUTOTEST(fft_48, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x48,   fft_test_y48,   48); }
LIQUID_AUTOTEST(fft_63, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x63,   fft_test_y63,   63); }
LIQUID_AUTOTEST(fft_92, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x92,   fft_test_y92,   92); }
LIQUID_AUTOTEST(fft_96, "composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x96,   fft_test_y96,   96); }
LIQUID_AUTOTEST(fft_120,"composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x120,  fft_test_y120, 120); }
LIQUID_AUTOTEST(fft_130,"composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x130,  fft_test_y130, 130); }
LIQUID_AUTOTEST(fft_192,"composite transforms","fft",0.1) { testbench_fft(__q__, fft_test_x192,  fft_test_y192, 192); }


// 
// AUTOTESTS: prime transforms
//
extern float complex fft_test_x17[];
extern float complex fft_test_y17[];

extern float complex fft_test_x43[];
extern float complex fft_test_y43[];

extern float complex fft_test_x79[];
extern float complex fft_test_y79[];

extern float complex fft_test_x157[];
extern float complex fft_test_y157[];

extern float complex fft_test_x317[];
extern float complex fft_test_y317[];

extern float complex fft_test_x509[];
extern float complex fft_test_y509[];

LIQUID_AUTOTEST(fft_17, "prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x17,   fft_test_y17,   17); }
LIQUID_AUTOTEST(fft_43, "prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x43,   fft_test_y43,   43); }
LIQUID_AUTOTEST(fft_79, "prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x79,   fft_test_y79,   79); }
LIQUID_AUTOTEST(fft_157,"prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x157,  fft_test_y157, 157); }
LIQUID_AUTOTEST(fft_317,"prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x317,  fft_test_y317, 317); }
LIQUID_AUTOTEST(fft_509,"prime transforms","fft",0.1) { testbench_fft(__q__, fft_test_x509,  fft_test_y509, 509); }

