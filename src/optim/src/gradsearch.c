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
    float * v;                  // vector to optimize (externally allocated)
    unsigned int num_parameters;// ...
    float u;                    // utility at current position

    // properties
    float delta;                // gradient approximation step size
    float alpha;                // line search step size

    float * p;                  // gradient estimate
    float pnorm;                // L2-norm of gradient estimate

    utility_function utility;   // utility function pointer
    void * userdata;            // object to optimize (user data)
    int direction;              // search direction (minimize/maximimze utility)
};

// create a gradient search object
//   _userdata          :   user data object pointer
//   _v                 :   array of parameters to optimize
//   _num_parameters    :   array length (number of parameters to optimize)
//   _u                 :   utility function pointer
//   _minmax            :   search direction (0:minimize, 1:maximize)
//   _props             :   properties (see above)
gradsearch gradsearch_create(void *              _userdata,
                             float *             _v,
                             unsigned int        _num_parameters,
                             utility_function    _utility,
                             int                 _direction,
                             gradsearchprops_s * _props)
{
    gradsearch q = (gradsearch) malloc( sizeof(struct gradsearch_s) );

    // set user-defined properties
    q->userdata       = _userdata;
    q->v              = _v;
    q->num_parameters = _num_parameters;
    q->utility        = _utility;
    q->direction      = _direction;

    // set internal properties
    // TODO : make these user-configurable properties
    q->delta = 1e-6f;               // gradient approximation step size
    q->alpha = q->delta * 100.0f;   // linear search step size

    // allocate array for gradient estimate
    q->p = (float*) malloc(q->num_parameters*sizeof(float));
    q->pnorm = 0.0f;
    q->u = 0.0f;

    return q;
}

void gradsearch_destroy(gradsearch _q)
{
    // free gradient estimate array
    free(_q->p);

    // free main object memory
    free(_q);
}

// print status
void gradsearch_print(gradsearch _q)
{
    //printf("gradient search:\n");
    printf("u=%12.4e,|p|=%12.4e : {", _q->u, _q->pnorm);
    unsigned int i;
    for (i=0; i<_q->num_parameters; i++)
        printf("%8.4f", _q->v[i]);
    printf("}\n");
}

float gradsearch_step(gradsearch _q)
{
    unsigned int i;
    // compute gradient
    gradsearch_gradient(_q->utility, _q->userdata, _q->v, _q->num_parameters, _q->delta, _q->p);

    // normalize gradient vector
    _q->pnorm = gradsearch_norm(_q->p, _q->num_parameters);

    // run line search
    _q->alpha = gradsearch_linesearch(_q->utility, _q->userdata, _q->v, _q->num_parameters, _q->p, _q->alpha);

    // step in the negative direction of the gradient
    for (i=0; i<_q->num_parameters; i++)
        _q->v[i] = _q->v[i] - _q->alpha*_q->p[i];

    // evaluate utility at current position
    _q->u = _q->utility(_q->userdata, _q->v, _q->num_parameters);

    // return utility
    return _q->u;
}

// batch execution of gradient search : run many steps and stop
// when criteria are met
float gradsearch_execute(gradsearch   _q,
                         unsigned int _max_iterations,
                         float        _target_utility)
{
    unsigned int i;
    for (i=0; i<_max_iterations; i++) {
        // step gradient search algorithm
        gradsearch_step(_q);

        // check exit criteria
        if (_q->pnorm < 1e-6f) break;
    }

    // return curent utility
    return _q->u;
}


// 
// internal (generic functions)
//

// compute the gradient of a function at a particular point
//  _utility    :   user-defined function
//  _userdata   :   user-defined data object
//  _x          :   operating point, [size: _n x 1]
//  _n          :   dimensionality of search
//  _delta      :   step value for which to compute gradient
//  _gradient   :   resulting gradient
void gradsearch_gradient(utility_function _utility,
                         void  *          _userdata,
                         float *          _x,
                         unsigned int     _n,
                         float            _delta,
                         float *          _gradient)
{
    // operating point for evaluation
    float x_prime[_n];
    float u_prime;

    // evaluate function at current operating point
    float u0 = _utility(_userdata, _x, _n);
        
    unsigned int i;
    for (i=0; i<_n; i++) {
        // copy operating point
        memmove(x_prime, _x, _n*sizeof(float));
        
        // increment test vector by delta along dimension 'i'
        x_prime[i] += _delta;

        // evaluate new utility
        u_prime = _utility(_userdata, x_prime, _n);

        // compute gradient estimate
        _gradient[i] = (u_prime - u0) / _delta;
    }
}

// execute line search; loosely solve:
//    min phi(alpha) := f(_x - alpha*_grad)
//  _utility    :   user-defined function
//  _userdata   :   user-defined data object
//  _x          :   operating point, [size: _n x 1]
//  _n          :   dimensionality of search
float gradsearch_linesearch(utility_function _utility,
                            void  *          _userdata,
                            float *          _x,
                            unsigned int     _n,
                            float *          _p,
                            float            _alpha)
{
    // simple method: continue to increase alpha while
    // objective function decreases
    unsigned int i;

    // determine if we need to increase or decrease alpha
    //float u0 = _utility(_userdata, _x, _n);
    
    float x_prime[_n];

    // evaluate utility
    for (i=0; i<_n; i++)
        x_prime[i] = _x[i] - _alpha*_p[i];
    float u0 = _utility(_userdata, x_prime, _n);

    // evaluate utility with increased value for alpha
    _alpha *= 1.01f;
    for (i=0; i<_n; i++)
        x_prime[i] = _x[i] - _alpha*_p[i];
    float u1 = _utility(_userdata, x_prime, _n);

    // determine scaling factor
    float gamma = (u1 > u0) ? 0.5f : 2.0f;

    do {
        // increase alpha
        _alpha *= gamma;

        // retain old estimate
        u0 = u1;

        for (i=0; i<_n; i++)
            x_prime[i] = _x[i] - _alpha*_p[i];

        // evaluate utility
        u1 = _utility(_userdata, x_prime, _n);
    } while (u1 < u0);

    return _alpha / gamma;
}

// normalize vector, returning its l2-norm
float gradsearch_norm(float *      _v,
                      unsigned int _n)
{
    float vnorm = 0.0f;

    unsigned int i;
    for (i=0; i<_n; i++)
        vnorm += _v[i]*_v[i];

    vnorm = sqrtf(vnorm);

    for (i=0; i<_n; i++)
        _v[i] /= vnorm;

    return vnorm;
}

