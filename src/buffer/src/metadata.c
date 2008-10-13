//
// Metadata
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "metadata_internal.h"

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
    unsigned int num_slots;
};

metadata metadata_create()
{
    metadata m = (metadata) malloc(sizeof(struct metadata_s));
    m->num_keys = 0;
    m->num_slots = 1;
    m->keys = (struct key_s *) malloc((m->num_slots)*sizeof(struct key_s));

    return m;
}

void metadata_destroy(metadata _m)
{
    free(_m->keys);
    free(_m);
}

void metadata_add_key(metadata _m, char * _name, float _value)
{
    if (_m->num_keys == _m->num_slots)
        metadata_increase_mem(_m);

    unsigned int i = _m->num_keys;
    strcpy(_m->keys[i].name, _name);
    _m->keys[i].value = _value;
    _m->keys[i].uuid = i;
    _m->num_keys++;
}

void metadata_set_key(metadata _m, char * _name, float _value)
{
    unsigned int id = metadata_get_key_id(_m, _name);
    _m->keys[id].value = _value;
}

float metadata_get_key(metadata _m, char * _name)
{
    unsigned int id = metadata_get_key_id(_m, _name);
    return _m->keys[id].value;
}

void metadata_print(metadata _m)
{
    unsigned int i;
    printf("metadata [%u keys] :\n", _m->num_keys);
    for (i=0; i<_m->num_keys; i++)
        printf(" %u\t: %6.2f\t(%s)\n", _m->keys[i].uuid, _m->keys[i].value, _m->keys[i].name);
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

unsigned int metadata_get_key_id(metadata _m, char * _name)
{
    if (_m->num_keys == 0) {
        printf("error: metadata_get_key_id(), no available keys\n");
        return 0;
    }

    unsigned int i;
    for (i=0; i<_m->num_keys; i++) {
        if (strcmp(_name, _m->keys[i].name)==0)
            return i;
    }
    printf("error: metadata_get_key_id() could not find key %s\n", _name);
    return 0;
}

void metadata_increase_mem(metadata _m)
{
    _m->num_slots += 2;
    _m->keys = (struct key_s *) realloc(
        (void*)(_m->keys),
        (_m->num_slots)*sizeof(struct key_s)
    );
}
