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
// Complex fixed-point logarithmic functions
//  cqtype_cexp()
//  cqtype_clog()
//  cqtype_csqrt()
//  cqtype_cpow()
//

#include "liquidfpm.internal.h"

// compute complex exponent
//      exp(x) = exp(x.real) * [cos(x.imag) + j*sin(x.imag)]
CQ(_t) CQ(_cexp)(CQ(_t) _x)
{
    // number of iterations (precision)
    unsigned int _precision = Q(_bits);

    // run CORDIC to compute sin|cos from angle
    // convert angle to qtype format (multiply by scaling factor)
    //      theta = _x.real * 2^(intbits-2) / pi
    Q(_t) theta = Q(_mul)(_x.imag, Q(_angle_scalar));
    Q(_t) qsin;     // pure in-phase component
    Q(_t) qcos;     // pure quadrature component
    Q(_sincos_cordic)(theta, &qsin, &qcos, _precision);

    // compute scaling factor
    Q(_t) scale = Q(_exp_shiftadd)(_x.real, _precision);

    // complex exponent
    CQ(_t) cexp = { Q(_mul)(scale,qcos), Q(_mul)(scale,qsin) };
    return cexp;
}

// compute complex exponent (purely imaginary exponent, phase
// representation)
//      exp(j*theta) = cos(theta) + j*sin(theta)
CQ(_t) CQ(_cexpj)(Q(_t) _theta)
{
    // number of iterations (precision)
    unsigned int _precision = Q(_bits);

    // run CORDIC to compute sin|cos from angle
    CQ(_t) y;
    Q(_sincos_cordic)(_theta, &y.imag, &y.real, _precision);
    return y;
}

// compute complex logarithm:
//      log(x) = log(|x|) + j*arg(x)
CQ(_t) CQ(_clog)(CQ(_t) _x)
{
    // number of iterations (precision)
    unsigned int _precision = Q(_bits);

    Q(_t) r;        // magnitude
    Q(_t) theta;    // angle

    // run CORDIC to compute both magnitude and angle of
    // complex phasor
    Q(_atan2_cordic)(_x.imag, _x.real, &r, &theta, _precision);

    // complex logarithm
    CQ(_t) clog = { Q(_log_shiftadd)(Q(_abs)(r),_precision),
                    Q(_div)(theta, Q(_angle_scalar)) };
    return clog;
}


// compute square root
CQ(_t) CQ(_csqrt)(CQ(_t) _x)
{
    // number of iterations (precision)
    unsigned int _precision = Q(_bits);

    // compute x = r exp(j*theta)
    Q(_t) r, theta;
    Q(_atan2_cordic)(_x.imag, _x.real, &r, &theta, _precision);

    // sqrt(x) = sqrt(r) * exp( j*theta/2 )

    // run CORDIC to compute sin|cos from angle
    Q(_t) qsin;
    Q(_t) qcos;
    Q(_sincos_cordic)(theta>>1, &qsin, &qcos, _precision);

    // compute sqrt(r)
    Q(_t) mag = Q(_sqrt_newton)(r, _precision);

    // compute output
    CQ(_t) y;
    y.real = Q(_mul)(mag,qcos);
    y.imag = Q(_mul)(mag,qsin);
    
    return y;
}

// compute complex power
//    b^(x) = 2^(x * log2(b))
//          = exp{x * ln(b)}
CQ(_t) CQ(_cpow)(CQ(_t) _b, CQ(_t) _x)
{
    // number of iterations (precision)
    //unsigned int _precision = Q(_bits);

    // compute ln(b)
    CQ(_t) lnb = CQ(_clog)(_b);

    // compute x*ln(b)
    CQ(_t) xlnb = CQ(_mul)(_x, lnb);

    // compute exp{x * ln(b)} and return
    return CQ(_cexp)(xlnb);
}
