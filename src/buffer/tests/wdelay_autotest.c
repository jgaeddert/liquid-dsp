/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: wdelayf
//
void autotest_wdelayf()
{
    float v;    // reader
    unsigned int i;

    // create wdelay
    // wdelay: 0 0 0 0
    wdelayf w = wdelayf_create(4);

    wdelayf_read(w, &v);
    CONTEND_EQUALITY(v, 0);

    float x0[10]      = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    float y0_test[10] = {0, 0, 0, 0, 1, 2, 3, 4, 5, 6};
    float y0[10];

    for (i=0; i<10; i++) {
        wdelayf_read(w, &y0[i]);
        wdelayf_push(w,  x0[i]);
        //printf("%3u : %6.2f (%6.2f)\n", i, y0[i], y0_test[i]);
    }
    // 7 8 9 10
    CONTEND_SAME_DATA(y0, y0_test, 10*sizeof(float));

    // re-create wdelay object
    // wdelay: 0 0 7 8 9 10
    w = wdelayf_recreate(w,6);

    float x1[10]     = {3, 4, 5, 6, 7, 8, 9, 2, 2, 2};
    float y1_test[10]= {0, 0, 7, 8, 9, 10,3, 4, 5, 6};
    float y1[10];
    for (i=0; i<10; i++) {
        wdelayf_read(w, &y1[i]);
        wdelayf_push(w,  x1[i]);
        //printf("%3u : %6.2f (%6.2f)\n", i, y1[i], y1_test[i]);
    }
    // wdelay: 7 8 9 2 2 2
    CONTEND_SAME_DATA(y1, y1_test, 10*sizeof(float));

    // re-create wdelay object
    // wdelay: 8 9 2 2 2
    w = wdelayf_recreate(w,5);

    float x2[10]     = {1, 1, 1, 1, 1, 1, 1, 2, 3, 4};
    float y2_test[10]= {8, 9, 2, 2, 2, 1, 1, 1, 1, 1};
    float y2[10];

    for (i=0; i<10; i++) {
        wdelayf_read(w, &y2[i]);
        wdelayf_push(w,  x2[i]);
        //printf("%3u : %6.2f (%6.2f)\n", i, y1[i], y1_test[i]);
    }
    // wdelay: 1 1 2 3 4
    CONTEND_SAME_DATA(y2, y2_test, 10*sizeof(float));

    // destroy object
    wdelayf_destroy(w);
}

