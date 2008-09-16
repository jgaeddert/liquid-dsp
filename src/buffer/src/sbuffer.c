//
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sbuffer.h"

struct sbuffer_s {
    float * v;
    unsigned int len;
    //unsigned int N;
    unsigned int num_elements;

    // mutex/semaphore
};

#define sbuffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])
void sbuffer_linearize(sbuffer _sb);

sbuffer sbuffer_create(unsigned int _n)
{
    sbuffer sb = (sbuffer) malloc(sizeof(struct sbuffer_s));
    sb->len = _n;

    sb->v = (float*) malloc((sb->len)*sizeof(float));
    sb->num_elements = 0;

    return sb;
}

void sbuffer_destroy(sbuffer _sb)
{
    free(_sb->v);
    free(_sb);
}

void sbuffer_print(sbuffer _sb)
{
    printf("buffer [%u elements] :\n", _sb->num_elements);
    unsigned int i;
    for (i=0; i<_sb->num_elements; i++) {
        printf("%u\t: %f\n", i, _sb->v[i]);
        //printf("%u\t: %f\n", i, sbuffer_fast_access(_sb,i));
    }
}

void sbuffer_debug_print(sbuffer _sb)
{
#if 0
    printf("buffer [%u elements] :\n", _sb->num_elements);
    unsigned int i;
    for (i=0; i<_sb->len; i++) {
        // print read index pointer
        if (i==_sb->read_index)
            printf("<r>");

        // print write index pointer
        if (i==_sb->write_index)
            printf("<w>");

        // print buffer value
        printf("\t%u\t: %f\n", i, _sb->v[i]);
    }
    printf("----------------------------------\n");

    // print excess buffer memory
    for (i=_sb->len; i<_sb->N; i++) {
        printf("\t%u\t: %f\n", i, _sb->v[i]);
    }
#endif
}

void sbuffer_clear(sbuffer _sb)
{
    _sb->num_elements = 0;
}

void sbuffer_read(sbuffer _sb, float ** _v, unsigned int *_n)
{
    printf("sbuffer_read() reading %u elements\n", _sb->num_elements);
    *_v = _sb->v;
    *_n = _sb->num_elements;
}

void sbuffer_release(sbuffer _sb)
{
    sbuffer_clear(_sb);
}

void sbuffer_write(sbuffer _sb, float * _v, unsigned int _n)
{
    //
    if (_n > (_sb->len - _sb->num_elements)) {
        printf("error: sbuffer_write(), cannot write more elements than are available\n");
        return;
    }

    memcpy(_sb->v + _sb->num_elements, _v, _n*sizeof(float));
    _sb->num_elements += _n;
}


