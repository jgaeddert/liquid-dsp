/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.internal.h"

#define LIQUID_QUASINEWTON_SEARCH_GAMMA_MIN 0.000001

quasinewton_search quasinewton_search_create(
    void* _obj,
    float* _v,
    unsigned int _num_parameters,
    utility_function _u,
    int _minmax)
{
    quasinewton_search q = (quasinewton_search) malloc( sizeof(struct quasinewton_search_s) );

    // initialize public values
    q->delta = 1e-6f;//_delta;
    q->gamma = 1e-3f;//_gamma;
    q->dgamma = 0.99f;
    q->gamma_hat = q->gamma;

    q->obj = _obj;
    q->v = _v;
    q->num_parameters = _num_parameters;
    q->get_utility = _u;
    q->minimize = ( _minmax == LIQUID_OPTIM_MINIMIZE ) ? 1 : 0;

    // initialize internal memory arrays
    q->B        = (float*) calloc( q->num_parameters*q->num_parameters, sizeof(float));
    q->p        = (float*) calloc( q->num_parameters, sizeof(float) );
    q->gradient = (float*) calloc( q->num_parameters, sizeof(float) );
    q->gradient0= (float*) calloc( q->num_parameters, sizeof(float) );
    q->v_prime  = (float*) calloc( q->num_parameters, sizeof(float) );
    q->dv       = (float*) calloc( q->num_parameters, sizeof(float) );
    q->utility = q->get_utility(q->obj, q->v, q->num_parameters);

    quasinewton_search_reset(q);

    return q;
}

void quasinewton_search_destroy(quasinewton_search _q)
{
    free(_q->B);

    free(_q->p);
    free(_q->gradient);
    free(_q->gradient0);
    free(_q->v_prime);
    free(_q->dv);
    free(_q);
}

void quasinewton_search_print(quasinewton_search _q)
{
    printf("[%.3f] ", _q->utility);
    unsigned int i;
    for (i=0; i<_q->num_parameters; i++)
        printf("%.3f ", _q->v[i]);
    printf("\n");
}

void quasinewton_search_reset(quasinewton_search _q)
{
    _q->gamma_hat = _q->gamma;

    // set B to identity matrix
    unsigned int i,j,n=0;
    for (i=0; i<_q->num_parameters; i++) {
        for (j=0; j<_q->num_parameters; j++) {
            _q->B[n++] = (i==j) ? 1.0f : 0.0f;
        }
    }
}

void quasinewton_search_step(quasinewton_search _q)
{
    unsigned int i;
    unsigned int n = _q->num_parameters;

    // compute normalized gradient vector
    quasinewton_search_compute_gradient(_q);
    quasinewton_search_normalize_gradient(_q);

    // TODO : perform line search to find optimal gamma
    _q->gamma_hat *= _q->dgamma;

    // compute search direction
    fmatrix_mul(_q->B,        n, n,
                _q->gradient, n, 1,
                _q->p,        n, 1);
    for (i=0; i<_q->num_parameters; i++)
        _q->p[i] = -_q->p[i];

    // override to gradient search
    for (i=0; i<_q->num_parameters; i++) _q->p[i] = -_q->gradient[i];

    // compute step vector
    for (i=0; i<_q->num_parameters; i++)
        _q->dv[i] = _q->gamma_hat * _q->p[i];

    // TODO update inverse Hessian approximation

    // apply change
    for (i=0; i<_q->num_parameters; i++)
        _q->v[i] += _q->dv[i];

    // store previous gradient
    memmove(_q->gradient0, _q->gradient, (_q->num_parameters)*sizeof(float));

    // update utility
    //_q->utility = utility_tmp;
}

float quasinewton_search_run(quasinewton_search _q,
                             unsigned int _max_iterations,
                             float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        quasinewton_search_step(_q);
        _q->utility = _q->get_utility(_q->obj, _q->v, _q->num_parameters);

    } while (
        optim_threshold_switch(_q->utility, _target_utility, _q->minimize) &&
        i < _max_iterations);

    return _q->utility;
}

// 
// internal
//

// compute gradient
void quasinewton_search_compute_gradient(quasinewton_search _q)
{
    unsigned int i;
    float f_prime;

    // reset v_prime
    memmove(_q->v_prime, _q->v, (_q->num_parameters)*sizeof(float));

    for (i=0; i<_q->num_parameters; i++) {
        _q->v_prime[i] += _q->delta;
        f_prime = _q->get_utility(_q->obj, _q->v_prime, _q->num_parameters);
        _q->v_prime[i] -= _q->delta;
        _q->gradient[i] = (f_prime - _q->utility) / _q->delta;
    }
}

// normalize gradient vector to unity
void quasinewton_search_normalize_gradient(quasinewton_search _q)
{
    // normalize gradient
    float sig = 0.0f;
    unsigned int i;
    for (i=0; i<_q->num_parameters; i++)
        sig += _q->gradient[i] * _q->gradient[i];

    sig = 1.0f / sqrtf(sig/(float)(_q->num_parameters));

    for (i=0; i<_q->num_parameters; i++)
        _q->gradient[i] *= sig;
}
