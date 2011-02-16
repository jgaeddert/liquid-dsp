/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic
 *                                Institute & State University
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
// Artificial neural network activation functions
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.experimental.h"

// 
// linear activation function
//
float ann_af_linear(float _mu, float _x)
{
    return _mu*_x;
}

float ann_df_linear(float _mu, float _x)
{
    return _mu;
}

// 
// logistic activation function
//
float ann_af_logistic(float _mu, float _x)
{
    return 1.0f / (1 + expf(-_mu*_x));
}

float ann_df_logistic(float _mu, float _x)
{
    float y = ann_af_logistic(_mu,_x);
    return _mu*y*(1-y);
}

// 
// tanh activation function
//
float ann_af_tanh(float _mu, float _x)
{
    return tanhf(_mu*_x);
}

float ann_df_tanh(float _mu, float _x)
{
    float sechf = 1.0f/coshf(_mu*_x);
    return _mu*sechf*sechf;
}

// 
// mu-law activation function
//
float ann_af_mulaw(float _mu, float _x)
{
    float y = logf(1 + _mu*fabsf(_x)) / logf(1 + _mu);
    return copysignf(y, _x);
}

float ann_df_mulaw(float _mu, float _x)
{
    // not exact derivative, but doesn't yield 'nan' result
    return ann_df_erf(_mu,_x);
    //return _mu/( logf(1+_mu)*(1+_mu*_x) );
}

// 
// erf (error function) activation function
//
float ann_af_erf(float _mu, float _x)
{
    return erf(_mu*_x);
}

float ann_df_erf(float _mu, float _x)
{
    return 2*_mu / sqrtf(M_PI) * expf(-(_mu*_x)*(_mu*_x));
}

