/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

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

struct metadata_s {
    struct key_s * keys;
    unsigned int num_keys;
    unsigned int num_slots;

    unsigned int revision;
};

metadata metadata_create()
{
    metadata m = (metadata) malloc(sizeof(struct metadata_s));
    m->num_keys = 0;
    m->num_slots = 1;
    m->keys = (struct key_s *) malloc((m->num_slots)*sizeof(struct key_s));

    m->revision = 0;

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
    printf("metadata [%u keys, rev %u] :\n", _m->num_keys, _m->revision);
    for (i=0; i<_m->num_keys; i++)
        printf(" %4u  : %12.6f   (%s)\n", _m->keys[i].uuid, _m->keys[i].value, _m->keys[i].name);
}

void metadata_add_key(metadata _m, char * _name, float _value)
{
    if (_m->num_keys == _m->num_slots)
        metadata_increase_mem(_m);

    unsigned int i = _m->num_keys;
    strncpy(_m->keys[i].name, _name, KEY_NAME_LENGTH-1);
    _m->keys[i].name[KEY_NAME_LENGTH-1] = '\0';
    _m->keys[i].value = _value;
    _m->keys[i].uuid = i;
    _m->num_keys++;

    _m->revision++;
}

void metadata_set_key(metadata _m, char * _name, float _value)
{
    unsigned int id = metadata_get_key_id(_m, _name);
    _m->keys[id].value = _value;

    _m->revision++;
}

float metadata_get_key(metadata _m, char * _name)
{
    unsigned int id = metadata_get_key_id(_m, _name);
    return _m->keys[id].value;
}

void metadata_update_key(metadata _m, const char * _name, float _val)
{
    // find key
    unsigned int i;
    for (i=0; i<_m->num_keys; i++) {
        if (strcmp(_name, _m->keys[i].name)==0) {
            _m->keys[i].value = _val;
            _m->revision++;
            return;
        }
    }

    printf("error: metadata_update_key(), could not find key %s\n", _name);
}

unsigned int metadata_get_revision(metadata _m)
{
    return _m->revision;
}

//
// internal methods
//
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
