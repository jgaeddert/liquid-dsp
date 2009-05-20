//
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIR_FILTER_USE_DOTPROD 1

// defined:
//  FIR_FILTER()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIR_FILTER(_s) {
    TC * h;
    unsigned int h_len;

#if FIR_FILTER_USE_DOTPROD
    WINDOW() w;
#else
    TI * v;
    unsigned int v_index;
#endif

    fir_prototype p;
};

FIR_FILTER() FIR_FILTER(_create)(TC * _h, unsigned int _n)
{
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

#if FIR_FILTER_USE_DOTPROD
    f->w = WINDOW(_create)(f->h_len);
#else
    f->v = malloc((f->h_len)*sizeof(TI));
#endif

    FIR_FILTER(_clear)(f);

    return f;
}

FIR_FILTER() FIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // use remez here

    return f;
}

FIR_FILTER() FIR_FILTER(_recreate)(FIR_FILTER() _f, TC * _h, unsigned int _n)
{
    unsigned int i;
    if (_n != _f->h_len) {
        // reallocate memory
        _f->h_len = _n;
        _f->h = (TC*) realloc(_f->h, (_f->h_len)*sizeof(TC));

#if FIR_FILTER_USE_DOTPROD
        _f->w = WINDOW(_recreate)(_f->w, _f->h_len);
#else
    _f->v = (TI*) realloc(_f->v, (_f->h_len)*sizeof(TI));
    // TODO: (bug) ensure window has proper state
    for (i=_n; i<_f->h_len; i++)
        _f->v[i] = 0;

    if (_n > _f->h_len)
        _f->v_index += (_n - _f->h_len)/2;
    else
        _f->v_index += _f->h_len + (_f->h_len - _n)/2;

    _f->v_index = (_f->v_index) % (_f->h_len);
#endif
    }

    // load filter in reverse order
    for (i=_n; i>0; i--)
        _f->h[i-1] = _h[_n-i];

    return _f;
}


void FIR_FILTER(_destroy)(FIR_FILTER() _f)
{
#if FIR_FILTER_USE_DOTPROD
    WINDOW(_destroy)(_f->w);
#else
    free(_f->v);
#endif
    free(_f->h);
    free(_f);
}

void FIR_FILTER(_clear)(FIR_FILTER() _f)
{
#if FIR_FILTER_USE_DOTPROD
    WINDOW(_clear)(_f->w);
#else
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        _f->v[i] = 0;
    _f->v_index = 0;
#endif
}

void FIR_FILTER(_print)(FIR_FILTER() _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL(_f->h[n-i-1]);
        printf("\n");
    }
}

void FIR_FILTER(_push)(FIR_FILTER() _f, TI _x)
{
#if FIR_FILTER_USE_DOTPROD
    WINDOW(_push)(_f->w, _x);
#else
    _f->v[ _f->v_index ] = _x;
    (_f->v_index)++;
    _f->v_index = (_f->v_index) % (_f->h_len);
#endif
}

void FIR_FILTER(_execute)(FIR_FILTER() _f, TO *_y)
{
#if FIR_FILTER_USE_DOTPROD
    TI *r;
    WINDOW(_read)(_f->w, &r);
    DOTPROD(_run)(_f->h, r, _f->h_len, _y);
#else
    TO y = 0;
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        y += _f->v[ (i+_f->v_index)%(_f->h_len) ] * _f->h[i];
    *_y = y;
#endif
}

unsigned int FIR_FILTER(_get_length)(FIR_FILTER() _f)
{
    return _f->h_len;
}

