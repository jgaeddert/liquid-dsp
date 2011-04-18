// file: doc/listings/matrix.example.c
#include <liquid/liquid.h>

int main() {
    // designate X as a 4 x 4 matrix
    float X[16] = {
       0.84382,  -2.38304,   1.43061,  -1.66604,
       3.99475,   0.88066,   4.69373,   0.44563,
       7.28072,  -2.06608,   0.67074,   9.80657,
       6.07741,  -3.93099,   1.22826,  -0.42142};
    matrixf_print(X,4,4);

    // L/U decomp (Doolittle's method)
    float L[16], U[16], P[16];
    matrixf_ludecomp_doolittle(X,4,4,L,U,P);
}

