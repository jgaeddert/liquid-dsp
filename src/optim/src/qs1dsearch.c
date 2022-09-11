/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.internal.h"

// quasi-Newton search object
struct qs1dsearch_s {
    float vn, va, v0, vb, vp;
    float un, ua, u0, ub, up;

    // values initialized?
    int init;

    // External utility function.
    liquid_utility_1d utility;   //
    void *            context;   // user-defined data pointer passed to utility callback
    int               direction; // search direction (minimize/maximimze utility)
    unsigned int      num_steps; // number of steps evaluated
};

// internal methods
int qs1dsearch_linesearch(qs1dsearch _q);

// create quasi-newton method search object
qs1dsearch qs1dsearch_create(liquid_utility_1d _utility,
                             void *            _context,
                             int               _direction)
{
    qs1dsearch q = (qs1dsearch) malloc( sizeof(struct qs1dsearch_s) );
    q->utility   = _utility;
    q->context   = _context;
    q->direction = _direction;

    // reset object and return
    qs1dsearch_reset(q);
    return q;
}

qs1dsearch qs1dsearch_copy(qs1dsearch _q)
{
    return NULL;
}

int qs1dsearch_destroy(qs1dsearch _q)
{
    return LIQUID_OK;
}

int qs1dsearch_print(qs1dsearch _q)
{
    printf("<liquid.qs1dsearch>\n");
    return LIQUID_OK;
}

int qs1dsearch_reset(qs1dsearch _q)
{
    _q->init = 0;
    return LIQUID_OK;
}

// perform line search
int qs1dsearch_init(qs1dsearch _q,
                    float      _v)
{
    // find appropriate search direction
    float step = 1e-12f;
    float vx = _v;
    float vy = _v + step;
    float ux = _q->utility(vx, _q->context);
    float uy = _q->utility(vy, _q->context);
    if ( (_q->direction == LIQUID_OPTIM_MINIMIZE && uy > ux) ||
         (_q->direction == LIQUID_OPTIM_MAXIMIZE && ux < uy) )
    {
        // swap search direction
        step = -step;
        float v_tmp = vx; vx = vy; vy = v_tmp;
        float u_tmp = ux; ux = uy; uy = u_tmp;
    }

    // continue stepping until metric degrades
    unsigned int i;
    float v_test = vy + step;
    float u_test = _q->utility(v_test, _q->context);
    for (i=0; i<100; i++) {
        v_test = vy + step;
        u_test = _q->utility(v_test, _q->context);
        printf(" %2u (step=%12.4e) : [%10.6f,%10.6f,%10.6f] : {%10.6f,%10.6f,%10.6f}\n",
                i, step, vx, vy, v_test, ux, uy, u_test);
        if ( (_q->direction == LIQUID_OPTIM_MINIMIZE && u_test > uy) ||
             (_q->direction == LIQUID_OPTIM_MAXIMIZE && u_test < uy) )
        {
            // skipped over optimum
            break;
        }

        // haven't hit max yet
        vx = vy;
        ux = uy;
        vy = v_test;
        uy = u_test;
        step *= 2.0f;
    }

    // set bounds
    _q->vn = vx;
    _q->v0 = vy;
    _q->vp = v_test;

    _q->un = ux;
    _q->u0 = uy;
    _q->up = u_test;
    _q->init = 1;
    return LIQUID_OK;
}

int qs1dsearch_init_bounds(qs1dsearch _q,
                           float      _vn,
                           float      _vp)
{
    // set bounds appropriately
    _q->vn = _vn < _vp ? _vn : _vp;
    _q->vp = _vn < _vp ? _vp : _vn;
    _q->v0 = 0.5f*(_vn + _vp);

    // evaluate utility
    _q->un = _q->utility(_q->vn, _q->context);
    _q->u0 = _q->utility(_q->v0, _q->context);
    _q->up = _q->utility(_q->vp, _q->context);

    _q->init = 1;

    // TODO: ensure v0 is optimum here
    return LIQUID_OK;
}

int qs1dsearch_step(qs1dsearch _q)
{
    // TODO: allow option for geometric mean?
    // compute new candidate points
    _q->va = 0.5f*(_q->vn + _q->v0);
    _q->vb = 0.5f*(_q->v0 + _q->vp);

    // evaluate utility
    _q->ua = _q->utility(_q->va, _q->context);
    _q->ub = _q->utility(_q->vb, _q->context);

    printf(" %3u [%7.3f,%7.3f,%7.3f,%7.3f,%7.3f] : {%7.3f,%7.3f,%7.3f,%7.3f,%7.3f}\n",
        _q->num_steps,
        _q->vn, _q->va, _q->v0, _q->vb, _q->vp,
        _q->un, _q->ua, _q->u0, _q->ub, _q->up);

    // [ (vn)  va  (v0)  vb  (vp) ]
    // optimum should be va, v0, or vb
    // TODO: check direction
    if (_q->ua < _q->u0 && _q->ua < _q->ub) {
        // va is optimum
        _q->vp = _q->v0; _q->up = _q->u0;
        _q->v0 = _q->va; _q->u0 = _q->ua;
    } else if (_q->u0 < _q->ua && _q->u0 < _q->ub) {
        // v0 is optimum
        _q->vn = _q->va; _q->un = _q->ua;
        _q->vp = _q->vb; _q->up = _q->ub;
    } else {
        // vb is optimum
        _q->vn = _q->v0; _q->un = _q->u0;
        _q->v0 = _q->vb; _q->u0 = _q->ub;
    }

    _q->num_steps++;
    return LIQUID_OK;
}

int qs1dsearch_execute(qs1dsearch _q)
{
    return LIQUID_OK;
}

unsigned int qs1dsearch_get_num_steps(qs1dsearch _q)
{
    return _q->num_steps;
}

float qs1dsearch_get_opt_v(qs1dsearch _q)
{
    return _q->v0;
}

float qs1dsearch_get_opt_u(qs1dsearch _q)
{
    return _q->u0;
}

//
// internal
//

// perform line search to get initial bounds
int qs1dsearch_linesearch(qs1dsearch _q)
{
    return LIQUID_OK;
}

