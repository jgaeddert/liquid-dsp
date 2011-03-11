// 
// math_poly_examples.c
//

#include <stdio.h>

#include "liquid.h"

void polyf_val_example();
void polyf_fit_example();
void polyf_fit_lagrange_example();

int main() {
    // run examples
    polyf_val_example();
    polyf_fit_example();
    polyf_fit_lagrange_example();

    printf("done.\n");
    return 0;
}

void polyf_val_example()
{
    printf("polyf_val_example:\n");
    float p[3] = {0.2f, 1.0f, 0.4f};
    float x = 1.3f;
    float y = polyf_val(p,3,x);
    printf("    >>> y = %12.8f\n", y);
}

void polyf_fit_example()
{
    printf("polyf_fit_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float y[4] = {0.85f, 3.07f, 5.07f, 7.16f};
    float p[2];
    polyf_fit(x,y,4,p,2);
    printf("    >>> p = {%12.8f, %12.8f}\n", p[0], p[1]);
}
void polyf_fit_lagrange_example()
{
    printf("polyf_fit_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float y[4] = {0.85f, 3.07f, 5.07f, 7.16f};
    float p[4];
    polyf_fit_lagrange(x,y,4,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}
