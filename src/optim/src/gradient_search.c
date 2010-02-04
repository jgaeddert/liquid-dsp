/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.internal.h"

#define LIQUID_GRADIENT_SEARCH_GAMMA_MIN 0.000001

// default parameters
#define LIQUID_GRADIENT_SEARCH_DEFAULT_DELTA    (1e-6f)
#define LIQUID_GRADIENT_SEARCH_DEFAULT_GAMMA    (0.002f)
#define LIQUID_GRADIENT_SEARCH_DEFAULT_ALPHA    (0.1f)
#define LIQUID_GRADIENT_SEARCH_DEFAULT_MU       (0.99f)

gradient_search gradient_search_create(
    void * _userdata,
    float * _v,
    unsigned int _num_parameters,
    utility_function _u,
    int _minmax)
{
    return gradient_search_create_advanced(_userdata,
                                           _v,
                                           _num_parameters,
                                           LIQUID_GRADIENT_SEARCH_DEFAULT_DELTA,
                                           LIQUID_GRADIENT_SEARCH_DEFAULT_GAMMA,
                                           LIQUID_GRADIENT_SEARCH_DEFAULT_ALPHA,
                                           LIQUID_GRADIENT_SEARCH_DEFAULT_MU,
                                           _u,
                                           _minmax);
}

gradient_search gradient_search_create_advanced(
    void * _userdata,
    float * _v,
    unsigned int _num_parameters,
    float _delta,
    float _gamma,
    float _alpha,
    float _mu,
    utility_function _u,
    int _minmax)
{
    gradient_search gs = (gradient_search) malloc( sizeof(struct gradient_search_s) );

    // initialize public values
    gs->delta = _delta;
    gs->gamma = _gamma;
    gs->mu    = _mu;
    gs->gamma_hat = gs->gamma;
    gs->alpha = _alpha;

    gs->userdata = _userdata;
    gs->v = _v;
    gs->num_parameters = _num_parameters;
    gs->get_utility = _u;
    gs->minimize = ( _minmax == LIQUID_OPTIM_MINIMIZE ) ? 1 : 0;

    // initialize internal memory arrays
    gs->gradient = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->v_prime  = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->dv_hat   = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->dv       = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->utility = 0.0f;

    return gs;
}

void gradient_search_destroy(gradient_search _g)
{
    free(_g->gradient);
    free(_g->v_prime);
    free(_g->dv_hat);
    free(_g->dv);
    free(_g);
}

void gradient_search_print(gradient_search _g)
{
    printf("[%.3f] ", _g->utility);
    unsigned int i;
    for (i=0; i<_g->num_parameters; i++)
        printf("%.3f ", _g->v[i]);
    printf("\n");
}

void gradient_search_reset(gradient_search _g)
{
    _g->gamma_hat = _g->gamma;
}

void gradient_search_step(gradient_search _g)
{
    // compute gradient vector (un-normalized)
    gradient_search_compute_gradient(_g);

    // normalize gradient vector
    gradient_search_normalize_gradient(_g);

    // compute vector step : retain [alpha]% of old gradient
    unsigned int i;
    for (i=0; i<_g->num_parameters; i++) {
        _g->dv[i] = _g->gamma_hat * _g->gradient[i] +
                    _g->dv_hat[i] * _g->alpha;
    }

    // store vector step
    for (i=0; i<_g->num_parameters; i++)
        _g->dv_hat[i] = _g->dv[i];

    // update optimum vector: optimization type determines polarity
    //   - (minimize)
    //   + (maximize)
    if (_g->minimize) {
        for (i=0; i<_g->num_parameters; i++)
            _g->v[i] -= _g->dv[i];
    } else {
        for (i=0; i<_g->num_parameters; i++)
            _g->v[i] += _g->dv[i];
    }

    // compute utility for this parameter set,
    float utility_tmp;
    utility_tmp = _g->get_utility(_g->userdata, _g->v, _g->num_parameters);

    // decrease gamma if utility did not improve from last iteration
    //if ( optim_threshold_switch(utility_tmp, _g->utility, _g->minimize) &&
    if ( optim_threshold_switch(utility_tmp, _g->utility, _g->minimize) &&
         _g->gamma_hat > LIQUID_GRADIENT_SEARCH_GAMMA_MIN )
    {
        _g->gamma_hat *= _g->mu;
    }

    // update utility
    _g->utility = utility_tmp;
}

// batch execution of gradient search : run many steps and stop
// when criteria are met
float gradient_search_execute(gradient_search _g,
                              unsigned int _max_iterations,
                              float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        gradient_search_step(_g);
        //_g->utility = _g->get_utility(_g->userdata, _g->v, _g->num_parameters);

    } while (
        optim_threshold_switch(_g->utility, _target_utility, _g->minimize) &&
        i < _max_iterations);

    return _g->utility;
}


// returns _u1 > _u2 if _minimize, _u1 < _u2 otherwise
int optim_threshold_switch(float _u1, float _u2, int _minimize)
{
    return _minimize ? _u1 > _u2 : _u1 < _u2;
}

// 
// internal
//

// compute the gradient vector (estimate)
void gradient_search_compute_gradient(gradient_search _g)
{
    // compute initial utility
    float u, u_prime;
    u = _g->get_utility(_g->userdata,
                        _g->v,
                        _g->num_parameters);

    // reset v_prime
    memmove(_g->v_prime, _g->v, _g->num_parameters*sizeof(float));

    unsigned int i;
    // compute gradient estimate on each dimension
    for (i=0; i<_g->num_parameters; i++) {
        // increment test vector by delta
        _g->v_prime[i] += _g->delta;

        // compute new utility
        u_prime = _g->get_utility(_g->userdata, 
                                  _g->v_prime,
                                  _g->num_parameters);

        // reset test vector
        _g->v_prime[i] = _g->v[i];

        // compute gradient estimate (slop of utility for the
        // i^th parameter)
        _g->gradient[i] = (u_prime - u) / _g->delta;
    }
}

// normalize gradient vector to unity
void gradient_search_normalize_gradient(gradient_search _g)
{
    // normalize gradient
    float sig = 0.0f;
    unsigned int i;
    for (i=0; i<_g->num_parameters; i++)
        sig += _g->gradient[i] * _g->gradient[i];

    if ( sig == 0.0f )
        return;

    sig = 1.0f / sqrtf(sig/(float)(_g->num_parameters));

    for (i=0; i<_g->num_parameters; i++)
        _g->gradient[i] *= sig;
}


