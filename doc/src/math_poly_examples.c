// 
// math_poly_examples.c
//

#include <stdio.h>

#include "liquid.h"

void polyf_val_example();
void polyf_fit_example();
void polyf_fit_lagrange_example();
void polyf_interp_lagrange_example();
void polyf_fit_lagrange_barycentric_example();
void polyf_val_lagrange_barycentric_example();
void polyf_expandroots_example();
void polyf_expandroots2_example();
void polyf_expandbinomial_example();
void polyf_expandbinomial_pm_example();

int main() {
    // run examples
    polyf_val_example();
    polyf_fit_example();
    polyf_fit_lagrange_example();
    polyf_interp_lagrange_example();
    polyf_fit_lagrange_barycentric_example();
    polyf_val_lagrange_barycentric_example();
    polyf_expandroots_example();
    polyf_expandroots2_example();
    polyf_expandbinomial_example();
    polyf_expandbinomial_pm_example();

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
    printf("polyf_fit_lagrange_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float y[4] = {0.85f, 3.07f, 5.07f, 7.16f};
    float p[4];
    polyf_fit_lagrange(x,y,4,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}

void polyf_interp_lagrange_example()
{
    printf("polyf_interp_lagrange_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float y[4] = {0.85f, 3.07f, 5.07f, 7.16f};
    float x0 = 0.5f;
    float y0 = polyf_interp_lagrange(x,y,4,x0);
    printf("    >>> y0 = %12.8f\n", y0);
}

void polyf_fit_lagrange_barycentric_example()
{
    printf("polyf_fit_lagrange_barycentric_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float w[4];
    polyf_fit_lagrange_barycentric(x,4,w);
    printf("    >>> w = {%12.8f, %12.8f, %12.8f, %12.8f}\n", w[0], w[1], w[2], w[3]);
}
void polyf_val_lagrange_barycentric_example()
{
    printf("polyf_val_lagrange_barycentric_example:\n");
    float x[4] = {0.0f,  1.0f,  2.0f,  3.0f};
    float y[4] = {0.85f, 3.07f, 5.07f, 7.16f};
    float w[4];
    polyf_fit_lagrange_barycentric(x,4,w);
    float x0 = 0.5f;
    float y0 = polyf_val_lagrange_barycentric(x,y,w,x0,4);
    printf("    >>> y0 = %12.8f\n", y0);
}

void polyf_expandroots_example()
{
    printf("polyf_expandroots_example:\n");
    float roots[3] = {1.0f, -2.0f, 3.0f};
    float p[4];
    polyf_expandroots(roots,3,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}

void polyf_expandroots2_example()
{
    printf("polyf_expandroots2_example:\n");
    float b[3] = { 2.0f, -3.0f, -1.0f};
    float a[3] = { 1.0f, -2.0f,  3.0f};
    float p[4];
    polyf_expandroots2(b,a,3,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}

void polyf_expandbinomial_example()
{
    printf("polyf_expandbinomial_example:\n");
    float p[4];
    polyf_expandbinomial(3,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}

void polyf_expandbinomial_pm_example()
{
    printf("polyf_expandbinomial_pm_example:\n");
    float p[4];
    polyf_expandbinomial_pm(2,1,p);
    printf("    >>> p = {%12.8f, %12.8f, %12.8f, %12.8f}\n", p[0], p[1], p[2], p[3]);
}

