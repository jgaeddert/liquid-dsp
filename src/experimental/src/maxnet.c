/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
#include <math.h>

#include "liquid.experimental.h"

#define MAXNET(name)    LIQUID_CONCAT(maxnet,name)
#define ANN(name)       LIQUID_CONCAT(ann,name)
#define ANNLAYER(name)  LIQUID_CONCAT(annlayer,name)
#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define DEBUG_MAXNET  0

// create maxnet object
//  _num_classes    :   number of distinct classes to identify
//  _structure      :   structure for each network [size: _num_layers x 1]
//  _num_layers     :   number of layers in _structure
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
    
    q->num_inputs  = _structure[0];
    q->num_classes = _num_classes;

    // initialize networks
    q->networks = (ANN()*) malloc( q->num_classes * sizeof(ANN()) );
    unsigned int i;
    for (i=0; i<q->num_classes; i++) {
        q->networks[i] = ANN(_create)(_structure,
                                      _num_layers,
                                      LIQUID_ANN_AF_TANH,
                                      LIQUID_ANN_AF_LINEAR);
        ANN(_init_random_weights)(q->networks[i]);
    }

    return q;
}

// destroy maxnet object
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

// print maxnet parameters
void MAXNET(_print)(MAXNET() _q)
{
    printf("maxnet:\n");
    printf("  num classes   :   %u\n", _q->num_classes);
}

// evaluate maxnet
//  _q      :   maxnet object
//  _x      :   multi-dimensional input [size: internal.num_inputs x 1]
//  _y      :   class network raw output [size: internal.num_classes x 1]
//  _class  :   index to maximum class (max(_y))
void MAXNET(_evaluate)(MAXNET() _q,
                       T * _x,
                       T * _y,
                       unsigned int * _class)
{
    // evaluate each network
    unsigned int i;
    float y_max = 0.0f;
#if DEBUG_MAXNET
    printf("[");
    for (i=0; i<_q->num_classes; i++)
        printf("%12.8f", _x[i]);
    printf("] > [");
#endif

    for (i=0; i<_q->num_classes; i++) {
        ANN(_evaluate)(_q->networks[i], _x, &_y[i]);

        if (_y[i] > y_max || i==0) {
            *_class = i;
            y_max = _y[i];
        }
    }
#if DEBUG_MAXNET
    for (i=0; i<_q->num_classes; i++)
        printf("%12.8f", _y[i]);
    printf("] (%3u)\n", *_class);
#endif
}

// train maxnet on single input
//  _q      :   maxnet object
//  _x      :   multi-dimensional input [size: internal.num_inputs x 1]
//  _class  :   output class
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

// train maxnet on batch of inputs
//  _q              :   maxnet object
//  _x              :   multi-dimensional input [size: _num_patterns x internal.num_inputs]
//  _class          :   output classes [size: _num_patterns x 1]
//  _num_patterns   :   number of patterns in input
//  _max_num_trials :   maximum number of trials
void MAXNET(_train_group)(MAXNET() _q,
                          float * _x,
                          unsigned int * _class,
                          unsigned int _num_patterns,
                          unsigned int _max_num_trials)
{
    float * x_train;            // pointer to training input
    unsigned int class_train;   // output class

    unsigned int i;
    for (i=0; i<_num_patterns; i++) {
        // assign input pointer
        x_train = &_x[i*_q->num_inputs];

        // assign output class
        class_train = _class[i];

        // run training algorithm
        MAXNET(_train)(_q, x_train, class_train);
    }
}

// compute maxnet root mean-squared error on input pattern set,
// expecting +1 for correct class, -1 for incorrect class.
//  _q              :   maxnet object
//  _x              :   input pattern [size: _num_patterns x internal.num_inputs]
//  _class          :   output class [size: _num_patterns x 1]
//  _num_patterns   :   number of input patterns
float MAXNET(_compute_rmse)(MAXNET() _q,
                            float * _x,
                            unsigned int * _class,
                            unsigned int _num_patterns)
{
    float y[_q->num_classes];       // maxnet soft output
    unsigned int c;                 // maxnet hard class output
    float e;                        // soft output error
    float rmse=0.0f;                // root mean-squared error
    unsigned int num_errors = 0;

    unsigned int i;
    for (i=0; i<_num_patterns; i++) {
        // evaluate maxnet
        MAXNET(_evaluate)(_q, &_x[i*_q->num_inputs], y, &c);

        // observe output from each network
        unsigned int j;
        float s;
        for (j=0; j<_q->num_classes; j++) {
            s = _class[i] == j ? 1.0f : -1.0f;
            e = y[j] - s;
            rmse += e*e;
        }
        num_errors += (c == _class[i]) ? 0 : 1;
    }
    rmse /= (float)_num_patterns * (float)(_q->num_classes);
    rmse = sqrtf(rmse);

    //printf("errors : %6u / %6u\n", num_errors, _num_patterns);

    return rmse;
}

