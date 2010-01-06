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
    
    return q;
}

void MAXNET(_destroy)(MAXNET() _q)
{
    free(_q);
}

void MAXNET(_print)(MAXNET() _q)
{
    printf("maxnet:\n");
}

void MAXNET(_evaluate)(MAXNET() _q,
                       T * _x,
                       T * _y,
                       unsigned int * _class)
{
}

void MAXNET(_train)(MAXNET() _q,
                    float * _x,
                    unsigned int * _class,
                    unsigned int _num_patterns,
                    unsigned int _max_num_trials)
{
}

