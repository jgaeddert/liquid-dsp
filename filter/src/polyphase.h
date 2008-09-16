//
// Polyphase filters
// 

// firpfb: fininte impulse response polyphase filter bank
typedef struct firpfb_s * firpfb;

firpfb firpfb_create(/*specs*/);

void firpfb_destroy(firpfb _f);

void firpfb_print(firpfb _f);

void firpfb_execute(firpfb _f, float *_x, float *_y, unsigned int _i);


