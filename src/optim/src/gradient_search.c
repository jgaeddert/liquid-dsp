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
#include "liquid.internal.h"

#define LIQUID_GRADIENT_SEARCH_GAMMA_MIN 0.000001

gradient_search gradient_search_create(
    void* _obj,
    float* _v,
    unsigned int _num_parameters,
    utility_function _u,
    int _minmax)
{
    return gradient_search_create_advanced(
        _obj,
        _v,
        _num_parameters,
        1e-6f,  // delta
        0.002f, // gamma
        _u,
        _minmax
        );
}

gradient_search gradient_search_create_advanced(
    void* _obj,
    float* _v,
    unsigned int _num_parameters,
    float _delta,
    float _gamma,
    utility_function _u,
    int _minmax)
{
    gradient_search gs = (gradient_search) malloc( sizeof(struct gradient_search_s) );

    // initialize public values
    gs->delta = _delta;
    gs->gamma = _gamma;
    gs->dgamma = 0.99f;
    gs->gamma_hat = gs->gamma;

    gs->obj = _obj;
    gs->v = _v;
    gs->num_parameters = _num_parameters;
    gs->get_utility = _u;
    gs->minimize = ( _minmax == LIQUID_OPTIM_MINIMIZE ) ? 1 : 0;

    // initialize internal memory arrays
    gs->gradient = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->v_prime  = (float*) calloc( gs->num_parameters, sizeof(float) );
    gs->utility = gs->get_utility(gs->obj, gs->v, gs->num_parameters);

    return gs;
}

void gradient_search_destroy(gradient_search _g)
{
    free(_g->gradient);
    free(_g->v_prime);
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
    // compute gradient on each dimension
    float f_prime;

    unsigned int i;
    // reset v_prime
    for (i=0; i<_g->num_parameters; i++)
        _g->v_prime[i] = _g->v[i];

    for (i=0; i<_g->num_parameters; i++) {
        _g->v_prime[i] += _g->delta;
        f_prime = _g->get_utility(_g->obj, _g->v_prime, _g->num_parameters);
        _g->v_prime[i] -= _g->delta;
        _g->gradient[i] = (f_prime - _g->utility) / _g->delta;
    }

    int continue_loop = 1;
    float utility_tmp;

    while (continue_loop) {
        // update v; polarity determines optimization type:
        //   - (minimum)
        //   + (maximum)
        if (_g->minimize) {
            for (i=0; i<_g->num_parameters; i++)
                _g->v[i] = _g->v_prime[i] - (_g->gamma_hat * _g->gradient[i]);
        } else {
            for (i=0; i<_g->num_parameters; i++)
                _g->v[i] = _g->v_prime[i] + (_g->gamma_hat * _g->gradient[i]);
        }

        // compute utility for this parameter set,
        utility_tmp = _g->get_utility(_g->obj, _g->v, _g->num_parameters);

        // decrease gamma if utility did not improve from last iteration
        if ( optim_threshold_switch(utility_tmp, _g->utility, _g->minimize) &&
             _g->gamma_hat > LIQUID_GRADIENT_SEARCH_GAMMA_MIN )
        {
            _g->gamma_hat *= _g->dgamma;
        } else {
            //continue_loop = 0;
        }
        continue_loop = 0;
    }

    // update utility
    _g->utility = utility_tmp;
}

float gradient_search_run(gradient_search _g, unsigned int _max_iterations, float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        gradient_search_step(_g);
        _g->utility = _g->get_utility(_g->obj, _g->v, _g->num_parameters);

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

