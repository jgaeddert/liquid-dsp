//
// Static buffer
//

#ifndef __MODULE_SBUFFER_H__
#define __MODULE_SBUFFER_H__

typedef struct sbuffer_s * sbuffer;

sbuffer sbuffer_create(unsigned int _n);

void sbuffer_destroy(sbuffer _sb);

void sbuffer_print(sbuffer _sb);

void sbuffer_debug_print(sbuffer _sb);

void sbuffer_clear(sbuffer _sb);

// read at least *_n elements, return actual number available
void sbuffer_read(sbuffer _sb, float ** _v, unsigned int *_n);

void sbuffer_release(sbuffer _sb);

void sbuffer_write(sbuffer _sb, float * _v, unsigned int _n);
//void sbuffer_force_write(sbuffer _sb, float * _v, unsigned int _n);

#endif // __MODULE_SBUFFER_H__

