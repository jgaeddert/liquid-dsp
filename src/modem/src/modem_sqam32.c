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
// modem_sqam32.c
//

// create a 'square' 32-QAM (on/off keying) modem object
modem modem_create_sqam32()
{
    modem q = (modem) malloc( sizeof(struct modem_s) );
    q->scheme = LIQUID_MODEM_SQAM32;

    modem_init(q, 5);

    // allocate memory for 8-point symbol map
    q->symbol_map = (float complex*) malloc( 8*sizeof(float complex) );
    memmove(q->symbol_map, modem_arb_sqam32, 8*sizeof(float complex));

    // set modulation, demodulation functions
    q->modulate_func = &modem_modulate_sqam32;
    q->demodulate_func = &modem_demodulate_sqam32;

    return q;
}

// modulate symbol with 'square' 32-QAM
void modem_modulate_sqam32(modem _q,
                           unsigned int _symbol_in,
                           float complex * _y)
{
    // strip off most-significant two bits (quadrant)
    unsigned int quad = (_symbol_in >> 3) & 0x03;
    
    // strip off least-significant 3 bits
    unsigned int s = _symbol_in & 0x07;
    float complex p = _q->symbol_map[s];
    
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
}

// demodulate 'square' 32-QAM
void modem_demodulate_sqam32(modem _q,
                             float complex _x,
                             unsigned int * _symbol_out)
{
    // determine quadrant and de-rotate to first quadrant
    // 10 | 00
    // ---+---
    // 11 | 01
    unsigned int quad = 2*(crealf(_x) < 0.0f) + (cimagf(_x) < 0.0f);
    
    float complex x_prime = _x;
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

    // find symbol in map closest to x_prime
    float dmin = 0.0f;
    float d = 0.0f;
    float complex x_hat = 0.0f;
    unsigned int i;
    for (i=0; i<8; i++) {
        d = cabsf(x_prime - _q->symbol_map[i]);
        if (i==0 || d < dmin) {
            dmin = d;
            *_symbol_out = i;
            x_hat = _q->symbol_map[i];
        }
    }

    // add quadrant bits
    *_symbol_out |= (quad << 3);

    // re-modulate symbol and store state
    modem_modulate_sqam32(_q, *_symbol_out, &_q->x_hat);
    _q->r = _x;
}

