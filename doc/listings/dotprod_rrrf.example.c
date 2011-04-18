// file: doc/listings/dotprod_rrrf.example.c
#include <liquid/liquid.h>

int main() {
    // create input arrays
    float x[] = { 1.0f,  2.0f,  3.0f,  4.0f,  5.0f};
    float v[] = { 0.1f, -0.2f,  1.0f, -0.2f,  0.1f};
    float y;

    // run the basic vector dot product, store in 'y'
    dotprod_rrrf_run(x,v,5,&y);

    // create dotprod object and execute, store in 'y'
    dotprod_rrrf q = dotprod_rrrf_create(v,5);
    dotprod_rrrf_execute(q,x,&y);
    dotprod_rrrf_destroy(q);
}


