//
//
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "metadata.h"

#define KEY_NAME_LENGTH 64

struct key_s {
    char name[KEY_NAME_LENGTH];
    unsigned int uuid;
    float value;
};

typedef struct key_s * key;

struct metadata_s {
    struct key_s * keys;
    unsigned int num_keys;
};

metadata metadata_create(unsigned int _n, ...)
{
    metadata m = (metadata) malloc(sizeof(struct metadata_s));
    //m->keys = (key) malloc(_n*sizeof(struct key_s));
    m->num_keys = _n;
    m->keys = (struct key_s *) malloc((m->num_keys)*sizeof(struct key_s));

    va_list argptr;
    va_start(argptr, _n);
    unsigned int i;
    for (i=0; i<_n; i++) {
        // create new key
        m->keys[i].uuid = i;

        char* name = va_arg(argptr, char *);
        double value = va_arg(argptr, double);

        strcpy(m->keys[i].name, name);
        m->keys[i].value = (float)(value);
    }
    va_end(argptr);

    return m;
}

void metadata_destroy(metadata _m)
{
    free(_m->keys);
    free(_m);
}

void metadata_print(metadata _m)
{
    unsigned int i;
    printf("metadata [%u keys] :\n", _m->num_keys);
    for (i=0; i<_m->num_keys; i++)
        printf(" %u\t: %4.2f (%s)\n", _m->keys[i].uuid, _m->keys[i].value, _m->keys[i].name);
}

void metadata_update(metadata _m, const char * _name, float _val)
{
    // find key
    unsigned int i;
    for (i=0; i<_m->num_keys; i++) {
        if (strcmp(_name, _m->keys[i].name)==0) {
            _m->keys[i].value = _val;
            return;
        }
    }

    printf("error: metadata_update(), could not find key %s\n", _name);
}


