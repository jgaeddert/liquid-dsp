//
//
//

#ifndef __MODULE_METADATA_H__
#define __MODULE_METADATA_H__

typedef struct metadata_s * metadata;
void metadata_destroy(metadata _m);
void metadata_print(metadata _m);

metadata metadata_create();

void metadata_add_key(metadata _m, char * _name, float _value);
void metadata_set_key(metadata _m, char * _name, float _value);
float metadata_get_key(metadata _m, char *_name);
void metadata_update_key(metadata _m, const char * _name, float _val);

unsigned int metadata_get_revision(metadata _m);


#endif // __MODULE_METADATA_H__

