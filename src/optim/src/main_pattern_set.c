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

