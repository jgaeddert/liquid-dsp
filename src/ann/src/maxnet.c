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

//
// maxnet (classifier)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define MAXNET(name)    LIQUID_CONCAT(maxnet,name)
#define ANN(name)       LIQUID_CONCAT(ann,name)
#define ANNLAYER(name)  LIQUID_CONCAT(annlayer,name)
#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define DEBUG_MAXNET  0

MAXNET() MAXNET(_create)(unsigned int _num_classes,
                         unsigned int * _structure,
                         unsigned int _num_layers)
{
    MAXNET() q = (MAXNET()) malloc(sizeof(struct MAXNET(_s)));

    // validate inputs
    if (_num_classes == 0) {
        fprintf(stderr,"error: maxnet_create(), must have more than one class\n");
        exit(1);
    } else if (_num_layers < 2) {
        fprintf(stderr,"error: maxnet_create(), networks must have at least 2 layers\n");
        exit(1);
    } else if (_structure[_num_layers-1] != 1) {
        fprintf(stderr,"error: maxnet_create(), network structure must have only one output\n");
        exit(1);
    }
    
    //q->num_inputs  = _num_inputs;
    q->num_classes = _num_classes;

    // initialize networks
    q->networks = (ANN()*) malloc( q->num_classes * sizeof(ANN()) );
    unsigned int i;
    for (i=0; i<q->num_classes; i++) {
        q->networks[i] = ANN(_create)(_structure, _num_layers);
        ANN(_init_random_weights)(q->networks[i]);
    }

    return q;
}

void MAXNET(_destroy)(MAXNET() _q)
{
    // destroy networks
    unsigned int i;
    for (i=0; i<_q->num_classes; i++)
        ANN(_destroy)(_q->networks[i]);
    free(_q->networks);

    // free main object
    free(_q);
}

void MAXNET(_print)(MAXNET() _q)
{
    printf("maxnet:\n");
    printf("  num classes   :   %u\n", _q->num_classes);
}

void MAXNET(_evaluate)(MAXNET() _q,
                       T * _x,
                       T * _y,
                       unsigned int * _class)
{
    // evaluate each network
    unsigned int i;
    float y_max = 0.0f;
    for (i=0; i<_q->num_classes; i++) {
        ANN(_evaluate)(_q->networks[i], _x, &_y[i]);

        if (_y[i] > y_max || i==0) {
            *_class = i;
        }
    }
}

void MAXNET(_train)(MAXNET() _q,
                    float * _x,
                    unsigned int _class)
{
    float y;
    unsigned int i;
    // train each network
    for (i=0; i<_q->num_classes; i++) {
        y = (i == _class) ? 1.0f : -1.0f;
        ANN(_train_bp)(_q->networks[i], _x, &y);
    }
}

void MAXNET(_train_group)(MAXNET() _q,
                          float * _x,
                          unsigned int * _class,
                          unsigned int _num_patterns,
                          unsigned int _max_num_trials)
{
    float y;
    unsigned int i;
    for (i=0; i<_num_patterns; i++) {
        // train each network
        unsigned int j;
        for (j=0; j<_q->num_classes; j++) {
            y = (j == _class[i]) ? 1.0f : -1.0f;
            //ANN(_train_bp)(_q->networks[j], x,
        }
    }
}

