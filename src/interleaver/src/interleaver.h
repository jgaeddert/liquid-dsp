//
// Block interleaver, operates on bytes
//

typedef struct interleaver_s * interleaver;

interleaver interleaver_create(unsigned int _n);

void interleaver_destroy(interleaver _i);

void interleaver_print(interleaver _i);

void interleaver_init_block(interleaver _i);

void interleaver_init_sequence(interleaver _i);

void interleaver_interleave(interleaver _i, unsigned char * _x, unsigned char * _y);

void interleaver_deinterleave(interleaver _i, unsigned char * _y, unsigned char * _x);

void interleaver_circshift_left(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_circshift_right(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_compute_bit_permutation(interleaver _q, unsigned int *_p);
void interleaver_debug_print(interleaver _i);

