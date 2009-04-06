//
// Matrix method definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


void MATRIX(_print)(T * _X, unsigned int _R, unsigned int _C)
{
    printf("matrix [%u x %u] : \n", _R, _C);
    unsigned int r,c;
    for (r=0; r<_R; r++) {
        for (c=0; c<_C; c++) {
            MATRIX_PRINT_ELEMENT(_X,_R,_C,r,c);
        }
        printf("\n");
    }
}

void MATRIX(_add)(unsigned int _R, unsigned int _C,
                  T * _X, T * _Y, T * _Z)
{
    unsigned int r, c;
    for (r=0; r<_R; r++) {
        for (c=0; c<_C; c++) {
            matrix_access(_Z,_R,_C,r,c) =
                matrix_access(_X,_R,_C,r,c) + matrix_access(_Y,_R,_C,r,c);
        }
    }
}

/*
void MATRIX(_multiply)(MATRIX() _x, MATRIX() _y, MATRIX() _z)
{
    // ensure lengths are valid
    if (!matrix_valid_size(_z,_x->M,_y->N)) {
        printf("error: matrix_multiply(), invalid dimensions\n");
        exit(0);
    }

    unsigned int m, n, i;
    for (m=0; m<_z->M; m++) {
        for (n=0; n<_z->N; n++) {
            // z(i,j) = dotprod( x(i,:), y(:,j) )
            T sum=0.0f;
#ifdef DEBUG
                printf("z(%u,%u) = ", m, n);
#endif
            for (i=0; i<_z->M; i++) {
                sum += matrix_fast_access(_x,m,i) *
                       matrix_fast_access(_y,i,n);
            }
            matrix_fast_access(_z,m,n) = sum;
        }
    }
}
*/


