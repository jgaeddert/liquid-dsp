// design recursive filter using gradient descent search
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_gradsearch_test.m"
#define DEBUG 0

// highly compressed linear activation function (like tanh but
// hugs lower bound between y=x and y=1 much more closely)
float activation(float _x)
    { float p=4.0f; return copysign(powf(tanhf(powf(fabsf(_x),p)),1./p),_x); }

typedef struct gs_s * gs;
struct gs_s {
    unsigned int n, L, r, nsos;     // filter order, etc.
    unsigned int vlen;              // search vector length
    float complex * zeros, * poles; // [size: L x 1]
    float           k, z0, p0;      // gain, additional zero/pole for odd-length order
    float * a, * b;         // second-order sections, [size: L+r x 3]
    // fft object, buffers
    // design parameters
};
gs    gs_create  (unsigned int _order);
void  gs_destroy (gs _q);
void  gs_unpack  (gs _q, float * _v, int _debug);
void  gs_expand  (gs _q, int _debug);
float gs_evaluate(gs _q);
float gs_callback(void * _q, float * _v, unsigned int _n);

int main()
{
    srand(time(NULL));
    gs q = gs_create(2);

    unsigned int n = q->vlen;
    float v[n]; // search vector
    unsigned int i;
    for (i=0; i<q->n; i++)
        v[i] = 0.0f;
    /*
    v[0] = 10;     v[1] = 2;
    v[2] =  0.633; v[3] = 1.6283;
    v[4] = 0.307;
    */
#if 0
    v[0] = -1.0f;  v[1] = 0.0f;
    v[2] = -0.03;  v[3] = 0.56;
    v[4] = 0.307;
#endif
    gs_unpack(q,v,1);
    gs_expand(q,1);
    //gs_destroy(q); return -1;

    unsigned int num_iterations = 250;

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create gradsearch object
    printf("creating gradsearch object...\n");
    gradsearch gs = gradsearch_create(q, v, n, gs_callback, LIQUID_OPTIM_MINIMIZE);

    // execute search one iteration at a time
    printf("executing search...\n");
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    fprintf(fid,"v = zeros(%u,%u);\n", n, num_iterations);
    for (i=0; i<num_iterations; i++) {
        float rmse = gs_callback(q,v,n);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, rmse);
        fprintf(fid,"v(:,%3u) = [", i+1);
        unsigned int j;
        for (j=0; j<n; j++)
            fprintf(fid,"%12.8f;", v[j]);
        fprintf(fid,"];\n");

        gradsearch_step(gs);

        gradsearch_print(gs);
    }

    // print results
    printf("\n");
    printf("%5u: ", num_iterations);
    gradsearch_print(gs);

    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(u,'-x');\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('gradient search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    gradsearch_destroy(gs);
    gs_destroy(q);

    return 0;
}

gs gs_create(unsigned int _order)
{
    gs q    = (gs) malloc(sizeof(struct gs_s));
    q->n    = _order;
    q->r    = (q->n) % 2;
    q->L    = (q->n - q->r)/2;
    q->vlen = 4*q->L + 1 + 2*q->r; // optimum vector length
    q->nsos = q->L + q->r;
    q->zeros= (float complex*)malloc(q->L*sizeof(float complex));
    q->poles= (float complex*)malloc(q->L*sizeof(float complex));
    q->b    = (float        *)malloc(3*q->nsos*sizeof(float));
    q->a    = (float        *)malloc(3*q->nsos*sizeof(float));
    return q;
}

void gs_destroy(gs _q)
{
    // free allocated memory
    free(_q->zeros);
    free(_q->poles);
    free(_q->b);
    free(_q->a);
    free(_q);
}

// unpack poles, zeros, and gain
// format: [{z-mag, z-phase, p-mag, p-phase}*L, k, {z-real, p-real}*r]
void gs_unpack(gs _q, float * _v, int _debug)
{
    unsigned int i;
    float x, y, r, t;
    for (i=0; i<_q->L; i++) {
        x = _v[4*i+0]; y = _v[4*i+1];
        r = activation(sqrtf(x*x+y*y));
        t = atan2f(y,x);
        _q->zeros[i] = r * cexpf(_Complex_I*t);
        x = _v[4*i+2]; y = _v[4*i+3];
        r = activation(sqrtf(x*x+y*y));
        t = atan2f(y,x);
        _q->poles[i] = r * cexpf(_Complex_I*t);
        //_q->zeros[i] = tanhf(_v[4*i+0])*cexpf(_Complex_I*_v[4*i+1]);
        //_q->poles[i] = tanhf(_v[4*i+2])*cexpf(_Complex_I*_v[4*i+3]);
    }
    _q->k = _v[4*_q->L]; // unpack gain
    if (_q->r) {
        _q->z0 = tanhf(_v[4*_q->L+1]);
        _q->p0 = tanhf(_v[4*_q->L+2]);
    }
    // debug: print values
    if (_debug) {
        printf("v : [");
        for (i=0; i<_q->vlen; i++)
            printf("%8.5f,", _v[i]);
        printf("]\n");
        printf("k : %12.10f\n", _q->k);
        for (i=0; i<_q->L; i++) {
            printf("[%2u] z:{%12.8f,%12.8f} p:{%12.8f,%12.8f}\n", i,
                    crealf(_q->zeros[i]), cimagf(_q->zeros[i]),
                    crealf(_q->poles[i]), cimagf(_q->poles[i]));
        }
        if (_q->r) {
            printf("[%2u] z:{%12.8f %12s} p:{%12.8f %12s}\n", _q->L,
                    _q->z0, "", _q->p0, "");
        }
    }
}

// expand second-order sections
void gs_expand(gs _q, int _debug)
{
    // distribute gain across all numerator second-order sections
    float g = powf(_q->k, 1.0f/(float)_q->nsos);
    unsigned int i;
    for (i=0; i<_q->L; i++) {
        // expand zeros
        float complex z = _q->zeros[i];
        _q->b[3*i+0] = g*1.0f;
        _q->b[3*i+1] = g*-2.0f*crealf(z);
        _q->b[3*i+2] = g*crealf(z*conjf(z));
        // expand poles
        float complex p = _q->poles[i];
        _q->a[3*i+0] = 1.0f;
        _q->a[3*i+1] = -2.0f*crealf(p);
        _q->a[3*i+2] = crealf(p*conjf(p));
    }
    // handle odd-order filters
    if (_q->r) {
        // expand zero
        _q->b[3*_q->L+0] = g*_q->z0;
        _q->b[3*_q->L+1] = g*_q->z0;
        _q->b[3*_q->L+2] = 0.0f;
        // expand pole
        _q->a[3*_q->L+0] = 1.0f;
        _q->a[3*_q->L+1] = -_q->p0;
        _q->a[3*_q->L+2] = 0.0f;
    }
    // debug: print second-order sections
    if (_debug) {
        printf("B [%u x 3]:\n", _q->nsos);
        for (i=0; i<_q->nsos; i++)
            printf("    %12.8f %12.8f %12.8f\n", _q->b[3*i+0], _q->b[3*i+1], _q->b[3*i+2]);
        printf("A [%u x 3]:\n", _q->nsos);
        for (i=0; i<_q->nsos; i++)
            printf("    %12.8f %12.8f %12.8f\n", _q->a[3*i+0], _q->a[3*i+1], _q->a[3*i+2]);
    }
}

float gs_evaluate(gs _q)
{
    return 0;
}

// gradient search error
float gs_callback(void * _context,
                  float * _v,
                  unsigned int _n)
{
    gs _q = (gs)_context;
    if (_n != _q->vlen) {
        fprintf(stderr,"search vector length mismatch\n");
        exit(1);
    }
    gs_unpack(_q, _v, DEBUG);
    gs_expand(_q, DEBUG);

    if (_q->nsos != 1)
        return 1e3f;

    float p = 2.0f;
    return
        powf(fabsf(0.30749798f - _q->b[0]), p)+
        powf(fabsf(0.61379653f - _q->b[1]), p)+
        powf(fabsf(0.30749798f - _q->b[2]), p)+
        powf(fabsf(1.00000000f - _q->a[0]), p)+
        powf(fabsf(0.06440119f - _q->a[1]), p)+
        powf(fabsf(0.31432679f - _q->a[2]), p);
}

#if 0
make examples/iirdes_example && ./examples/iirdes_example -t ellip -n 2 -o sos -f 0.25
make: `examples/iirdes_example' is up to date.
B [1 x 3] :
    0.30749798   0.61379653   0.30749798
A [1 x 3] :
    1.00000000   0.06440119   0.31432679
results written to iirdes_example.m.
done.


make examples/iirdes_example && ./examples/iirdes_example -t ellip -n 2 -o tf -f 0.25
make: `examples/iirdes_example' is up to date.
a[  0] =   1.00000000;
a[  1] =   0.06440119;
a[  2] =   0.31432679;
b[  0] =   0.30749798;
b[  1] =   0.61379653;
b[  2] =   0.30749798;
results written to iirdes_example.m.
done.

a = [1.00000000   0.06440119   0.31432679];
b = [0.30749798   0.61379653   0.30749798];

roots(a)
  -0.03220 + 0.55972i
  -0.03220 - 0.55972i

roots(b)
  -0.99805 + 0.06242i
  -0.99805 - 0.06242i


#endif
