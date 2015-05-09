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
// modem_sqam32.c
//

// create a 'square' 32-QAM modem object
MODEM() MODEM(_create_sqam32)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_SQAM32;

    MODEM(_init)(q, 5);

    // allocate memory for 8-point symbol map
    q->data.sqam32.map = (TC*) malloc( 8*sizeof(TC) );
    unsigned int i;
    for (i=0; i<8; i++) {
#if LIQUID_FPM
        q->data.sqam32.map[i] = CQ(_float_to_fixed)(modem_arb_sqam32[i]);
#else
        q->data.sqam32.map[i] = modem_arb_sqam32[i];
#endif
    }

    // set modulation, demodulation functions
    q->modulate_func   = &MODEM(_modulate_sqam32);
    q->demodulate_func = &MODEM(_demodulate_sqam32);

    // reset and return
    MODEM(_reset)(q);
    return q;
}

// modulate symbol with 'square' 32-QAM
void MODEM(_modulate_sqam32)(MODEM()      _q,
                             unsigned int _sym_in,
                             TC *         _y)
{
    // strip off most-significant two bits (quadrant)
    unsigned int quad = (_sym_in >> 3) & 0x03;
    
    // strip off least-significant 3 bits
    unsigned int s = _sym_in & 0x07;
    TC p = _q->data.sqam32.map[s];
    
#if LIQUID_FPM
    _y[0].real = quad & 0x02 ? -p.real : p.real;
    _y[0].imag = quad & 0x01 ? -p.imag : p.imag;
#else
    switch (quad) {
    case 0: *_y =  p;           return;
    case 1: *_y =  conjf(p);    return;
    case 2: *_y = -conjf(p);    return;
    case 3: *_y = -p;           return;
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_modulate_sqam32(), logic error\n");
        exit(1);
    }
#endif
}

// demodulate 'square' 32-QAM
void MODEM(_demodulate_sqam32)(MODEM()        _q,
                               TC             _x,
                               unsigned int * _sym_out)
{
    // determine quadrant and de-rotate to first quadrant
    // 10 | 00
    // ---+---
    // 11 | 01
#if LIQUID_FPM
    unsigned int quad = 2*(_x.real < 0) + (_x.imag < 0);
#else
    unsigned int quad = 2*(crealf(_x) < 0.0f) + (cimagf(_x) < 0.0f);
#endif
    
    TC x_prime = _x;
#if LIQUID_FPM
    x_prime.real = quad & 0x02 ? -_x.real : _x.real;
    x_prime.imag = quad & 0x01 ? -_x.imag : _x.imag;

    assert(x_prime.real >= 0);
    assert(x_prime.imag >= 0);
#else
    switch (quad) {
    case 0: x_prime = _x;           break;
    case 1: x_prime =  conjf(_x);   break;
    case 2: x_prime = -conjf(_x);   break;
    case 3: x_prime = -_x;          break;
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam32(), logic error\n");
        exit(1);
    }
    //printf(" x = %12.8f +j*%12.8f, quad = %1u, r = %12.8f + j*%12.8f\n",
    //        crealf(_x), cimagf(_x), quad, crealf(r), cimagf(r));
    assert(crealf(x_prime) >= 0.0f);
    assert(cimagf(x_prime) >= 0.0f);
#endif

    // find symbol in map closest to x_prime
    T dmin = 0.0f;
    T d = 0.0f;
    unsigned int i;
    for (i=0; i<8; i++) {
#if LIQUID_FPM
        d = CQ(_cabs2)( CQ(_sub)(x_prime, _q->data.sqam32.map[i]) );
#else
        d = cabsf(x_prime - _q->data.sqam32.map[i]);
#endif
        if (i==0 || d < dmin) {
            dmin = d;
            *_sym_out = i;
        }
    }

    // add quadrant bits
    *_sym_out |= (quad << 3);

    // re-modulate symbol and store state
    MODEM(_modulate_sqam32)(_q, *_sym_out, &_q->x_hat);
    _q->r = _x;
}

