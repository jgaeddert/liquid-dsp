//
// New buffer prototype
//

#include <stdbool.h>

typedef struct buffer_s * buffer;
typedef enum {
    error_none=0,
    error_buffer_unlocked,
    error_buffer_locked,
    error_buffer_overwritten,
    error_buffer_overread
} liquid_error;

struct buffer_s {
    float * v;
    unsigned int len;
    unsigned int N;

    // consumer
    unsigned int read_index;
    bool read_lock;
    unsigned int num_read_elements_locked;
    unsigned int num_read_elements_available;

    // producer
    unsigned int write_index;
    bool write_lock;
    unsigned int num_write_elements_locked;
    unsigned int num_write_elements_available;
};

// create/destroy/print methods
// NOTE _n is suggested starting size; buffer will realloc when necessary
buffer buffer_create(unsigned int _n);
void buffer_destroy(buffer _b);
void buffer_print(buffer _b);
void buffer_debug_print(buffer _b);

// accessor methods
unsigned int buffer_get_length(buffer _b);

// consumer methods
int buffer_consumer_lock_array(buffer _b, unsigned int *_n);
int buffer_consumer_read(buffer _b, float *_v, unsigned int _n);
int buffer_consumer_release_array(buffer _b);

// producer methods
int buffer_producer_lock_array(buffer _b, unsigned int *_n);
int buffer_producer_write(buffer _b, float *_v, unsigned int _n);
int buffer_producer_release_array(buffer _b);

