#include <liquid/liquid.h>

int main() {
    // create input arrays
    float x[] = {1, 2, 3, 4, 5};
    float v[] = {1, 1, 1, 1, 1};
    float y;

    // run the basic vector dot product, store in 'y'
    dotprod_rrrf_run(x,v,5,&y);

    // create dotprod object and execute, store in 'y'
    dotprod_rrrf q = dotprod_rrrf_create(v,5);
    dotprod_rrrf_execute(q,x,&y);
    dotprod_rrrf_destroy(q);
}


