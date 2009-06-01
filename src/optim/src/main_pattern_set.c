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

//
//
//

#include <stdio.h>
#include "optim.h"

int main() {
    float x[] = {1, 2, 3, 4, 5, 6};
    float y[] = {111, 222, 333};

    float *a, *b;

    optim_ps ps = optim_ps_create(2,1);

    optim_ps_print(ps);

    optim_ps_append_patterns(ps,x,y,3);
    optim_ps_print(ps);

    optim_ps_access(ps, 1, &a, &b);
    printf("1: %8.5f, %8.5f : %8.5f\n", a[0], a[1], b[0]);

    printf("deleting pattern 1...\n");
    optim_ps_delete_pattern(ps, 1);
    optim_ps_print(ps);

    optim_ps_clear(ps);
    optim_ps_print(ps);

    optim_ps_destroy(ps);

    printf("done.\n");
    return 0;
}

