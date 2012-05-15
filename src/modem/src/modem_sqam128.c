/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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
// modem_sqam128.c
//

// create a 'square' 128-QAM modem object
MODEM() MODEM(_create_sqam128)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_SQAM128;

    MODEM(_init)(q, 7);

    // allocate memory for 32-point symbol map
    q->data.sqam128.map = (TC*) malloc( 32*sizeof(TC) );
    unsigned int i;
    for (i=0; i<32; i++) {
#if LIQUID_FPM
        q->data.sqam128.map[i] = CQ(_float_to_fixed)(modem_arb_sqam128[i]);
#else
        q->data.sqam128.map[i] = modem_arb_sqam128[i];
#endif
    }

    // set modulation, demodulation functions
    q->modulate_func   = &MODEM(_modulate_sqam128);
    q->demodulate_func = &MODEM(_demodulate_sqam128);

    return q;
}

// modulate symbol with 'square' 128-QAM
void MODEM(_modulate_sqam128)(MODEM()      _q,
                              unsigned int _sym_in,
                              TC *         _y)
{
    // strip off most-significant two bits (quadrant)
    unsigned int quad = (_sym_in >> 5) & 0x03;
    
    // strip off least-significant 5 bits
    unsigned int s = _sym_in & 0x1f;
    TC p = _q->data.sqam128.map[s];
    
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
        fprintf(stderr,"error: modem_modulate_sqam128(), logic error\n");
        exit(1);
    }
#endif
}


// demodulate 'square' 128-QAM
void MODEM(_demodulate_sqam128)(MODEM()        _q,
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

    assert(x_prime.imag >= 0);
    assert(x_prime.imag >= 0);
#else
    switch (quad) {
    case 0: x_prime = _x;           break;
    case 1: x_prime =  conjf(_x);   break;
    case 2: x_prime = -conjf(_x);   break;
    case 3: x_prime = -_x;          break;
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam128(), logic error\n");
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
    for (i=0; i<32; i++) {
#if LIQUID_FPM
        d = CQ(_cabs2)( CQ(_sub)(x_prime, _q->data.sqam32.map[i]) );
#else
        d = cabsf(x_prime - _q->data.sqam128.map[i]);
#endif
        if (i==0 || d < dmin) {
            dmin = d;
            *_sym_out = i;
        }
    }

    // add quadrant bits
    *_sym_out |= (quad << 5);

    // re-modulate symbol and store state
    MODEM(_modulate_sqam128)(_q, *_sym_out, &_q->x_hat);
    _q->r = _x;
}

