//
// Optimization
//

#ifndef __LIQUID_OPTIM_H__
#define __LIQUID_OPTIM_H__

// optim pattern set (struct)
struct optim_ps_s {
    float *x, *y;
    unsigned int nx, ny, np;
    unsigned int na; // num allocated
};

typedef struct optim_ps_s * optim_ps;

optim_ps optim_ps_create(unsigned int _nx, unsigned int _ny);
void optim_ps_destroy(optim_ps _ps);
void optim_ps_print(optim_ps _ps);
void optim_ps_append_pattern(optim_ps _ps, float *_x, float *_y);
void optim_ps_append_patterns(optim_ps _ps, float *_x, float *_y, unsigned int _np);
void optim_ps_delete_pattern(optim_ps _ps, unsigned int _i);
void optim_ps_clear(optim_ps _ps);
void optim_ps_access(optim_ps _ps, unsigned int _i, float **_x, float **_y);

typedef void(*optim_target_function)(float *_x, float *_y, void *_p);
typedef float(*optim_obj_function)(optim_ps _ps, void *_p, optim_target_function _f);

#endif // __LIQUID_OPTIM_H__
