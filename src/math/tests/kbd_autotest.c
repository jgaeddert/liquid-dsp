/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Kaiser-Bessel derived window
//
void liquid_kbd_window_test(unsigned int _n,
                            float _beta)
{
    unsigned int i;
    float tol = 1e-3f;

    // compute window
    float w[_n];
    liquid_kbd_window(_n,_beta,w);

    // square window
    float w2[_n];
    for (i=0; i<_n; i++)
        w2[i] = w[i]*w[i];

    // ensure w[i]^2 + w[i+M]^2 == 1
    unsigned int M = _n/2;
    for (i=0; i<M; i++)
        CONTEND_DELTA(w2[i]+w2[(i+M)%_n], 1.0f, tol);

    // ensure sum(w[i]^2) == _n/2
    float sum=0.0f;
    for (i=0; i<_n; i++)
        sum += w2[i];
    CONTEND_DELTA(sum, 0.5f*_n, tol);
}

void autotest_kbd_n16() { liquid_kbd_window_test(16, 10.0f); }
void autotest_kbd_n32() { liquid_kbd_window_test(32, 20.0f); }
void autotest_kbd_n48() { liquid_kbd_window_test(48, 12.0f); }

