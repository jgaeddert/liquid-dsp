//
// Half-band decimator
//

typedef struct decim2_s * decim2;

// Create half-band decimator object
//   n  : filter length
//   fc : carrier frequency (-1 < fc < 1)
//   b  : bandwidth (0 < b < 0.5)
decim2 decim2_create(unsigned int _n, float _fc, float _b);

// Destroy half-band decimator object
void decim2_destroy(decim2 _d);

// Execute
void decim2_execute(decim2 _d, float * _x, unsigned int _x_len, float * _y, unsigned int _y_len);

