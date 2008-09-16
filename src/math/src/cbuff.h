//
// Circular buffer
//

struct cbuff_s;
typedef struct cbuff_s * cbuff;

// Create circular buffer object
cbuff cbuff_create(unsigned int _len);

// Destroy circular buffer object
void cbuff_destroy(cbuff _cb);

// Read _n elements
float * cbuff_read(cbuff _cb, unsigned int _n);

// Release _n elements
void cbuff_release(cbuff _cb, unsigned int _n);

// Write _n elements
int cbuff_write(cbuff _cb, float * _x, unsigned int _n);
