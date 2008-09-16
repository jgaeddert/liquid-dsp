//
// Matrix method prototypes
//

typedef struct matrix_s * matrix;

matrix matrix_create(unsigned int _M, unsigned int _N);

matrix matrix_copy(matrix _x);

void matrix_destroy(matrix _x);

void matrix_print(matrix _x);

void matrix_clear(matrix _x);

void matrix_dim(matrix _x, unsigned int *_M, unsigned int *_N);

void matrix_assign(matrix _x, unsigned int _m, unsigned int _n, float _value);

float matrix_access(matrix _x, unsigned int _m, unsigned int _n);

//void matrix_add(matrix _x, matrix _y, matrix _z);

void matrix_multiply(matrix _x, matrix _y, matrix _z);

void matrix_transpose(matrix _x);

void matrix_invert(matrix _x);

void matrix_lu_decompose(matrix _x, matrix _lower, matrix _upper);


