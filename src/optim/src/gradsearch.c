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

#define LIQUID_gradsearch_GAMMA_MIN 0.000001

// default gradsearch properties
static gradsearchprops_s gradsearchprops_default = {
    1e-6f,  // delta
    0.002f, // gamma
    0.1f,   // alpha
    0.99f   // mu
};

void gradsearchprops_init_default(gradsearchprops_s * _props)
{
    memmove(_props, &gradsearchprops_default, sizeof(gradsearchprops_s));
}

// gradient search algorithm (steepest descent) object
struct gradsearch_s {
    float* v;           // vector to optimize (externally allocated)
    unsigned int num_parameters;

    // properties
    gradsearchprops_s props;

    float gamma;        // nominal step size
    float delta;        // differential used to compute (estimate) derivative
    float alpha;        // momentum constant
    float mu;           // decremental gamma parameter

    float gamma_hat;    // step size (decreases each epoch)
    float* v_prime;     // temporary vector array
    float* dv;          // vector step
    float* dv_hat;      // vector step (previous iteration)

    float* gradient;    // gradient approximation
    float utility;      // current utility

    // External utility function.
    utility_function get_utility;
    void * userdata;    // object to optimize (user data)
    int minimize;       // minimize/maximimze utility (search direction)
};

// create a gradient search object
//   _userdata          :   user data object pointer
//   _v                 :   array of parameters to optimize
//   _num_parameters    :   array length (number of parameters to optimize)
//   _u                 :   utility function pointer
//   _minmax            :   search direction (0:minimize, 1:maximize)
//   _props             :   properties (see above)
gradsearch gradsearch_create(void * _userdata,
                             float * _v,
                             unsigned int _num_parameters,
                             utility_function _u,
                             int _minmax,
                             gradsearchprops_s * _props)
{
    gradsearch gs = (gradsearch) malloc( sizeof(struct gradsearch_s) );

    // initialize properties
    if (_props != NULL) {
        gs->delta = _props->delta;
        gs->gamma = _props->gamma;
        gs->mu    = _props->mu;
        gs->alpha = _props->alpha;
    } else {
        gs->delta = gradsearchprops_default.delta;
        gs->gamma = gradsearchprops_default.gamma;
        gs->mu    = gradsearchprops_default.mu;
        gs->alpha = gradsearchprops_default.alpha;
    }

    gs->gamma_hat = gs->gamma;

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

void gradsearch_destroy(gradsearch _g)
{
    free(_g->gradient);
    free(_g->v_prime);
    free(_g->dv_hat);
    free(_g->dv);
    free(_g);
}

void gradsearch_print(gradsearch _g)
{
    printf("[%.3f] ", _g->utility);
    unsigned int i;
    for (i=0; i<_g->num_parameters; i++)
        printf("%.3f ", _g->v[i]);
    printf("\n");
}

void gradsearch_reset(gradsearch _g)
{
    _g->gamma_hat = _g->gamma;
}

void gradsearch_step(gradsearch _g)
{
    // compute gradient vector (un-normalized)
    gradsearch_compute_gradient(_g);

    // normalize gradient vector
    gradsearch_normalize_gradient(_g);

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
         _g->gamma_hat > LIQUID_gradsearch_GAMMA_MIN )
    {
        _g->gamma_hat *= _g->mu;
    }

    // update utility
    _g->utility = utility_tmp;
}

// batch execution of gradient search : run many steps and stop
// when criteria are met
float gradsearch_execute(gradsearch _g,
                         unsigned int _max_iterations,
                         float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        gradsearch_step(_g);
        //_g->utility = _g->get_utility(_g->userdata, _g->v, _g->num_parameters);

    } while (
        optim_threshold_switch(_g->utility, _target_utility, _g->minimize) &&
        i < _max_iterations);

    return _g->utility;
}


// 
// internal
//

// compute the gradient vector (estimate)
void gradsearch_compute_gradient(gradsearch _g)
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
void gradsearch_normalize_gradient(gradsearch _g)
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


