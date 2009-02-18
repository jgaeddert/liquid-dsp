//
// P/N synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#include "liquid.h"

struct PNSYNC(_s) {
    unsigned int n; // sequence length
    WINDOW() sym;   // received symbols
    WINDOW() sym2;  // received symbols squared
    DOTPROD() dp;   // dot product
    TO rxy;         // cross correlation
};

PNSYNC() PNSYNC(_create)(unsigned int _n, TC * _v)
{
    PNSYNC() fs = (PNSYNC()) malloc(sizeof(struct PNSYNC(_s)));
    fs->n = _n;

    fs->sym  = WINDOW(_create)(fs->n);
    fs->sym2 = WINDOW(_create)(fs->n);

    TC h[fs->n];
    memmove(h, _v, (fs->n)*sizeof(TC));

    // compute signal energy and normalize
    unsigned int i;
    float e=0.0f;
    for (i=0; i<fs->n; i++)
        e += ABS(h[i]) * ABS(h[i]);

    e = sqrtf(e/(fs->n));

    for (i=0; i<fs->n; i++)
        h[i] /= e;

    fs->dp = DOTPROD(_create)(h,fs->n);

    return fs;
}

PNSYNC() PNSYNC(_create_msequence)(unsigned int _g)
{
    unsigned int m;
    switch (_g) {
    case LIQUID_MSEQUENCE_N3:       m=2;    break;
    case LIQUID_MSEQUENCE_N7:       m=3;    break;
    case LIQUID_MSEQUENCE_N15:      m=4;    break;
    case LIQUID_MSEQUENCE_N31:      m=5;    break;
    case LIQUID_MSEQUENCE_N63:      m=6;    break;
    case LIQUID_MSEQUENCE_N127:     m=7;    break;
    case LIQUID_MSEQUENCE_N255:     m=8;    break;
    case LIQUID_MSEQUENCE_N511:     m=9;    break;
    case LIQUID_MSEQUENCE_N1023:    m=10;   break;
    case LIQUID_MSEQUENCE_N2047:    m=11;   break;
    case LIQUID_MSEQUENCE_N4095:    m=12;   break;
    default:
        printf("error: framesync_create_msequence(), unsupported generator polynomial %.32x\n", _g);
        exit(0);
    }
    msequence ms = msequence_create(m);

    PNSYNC() fs = (PNSYNC()) malloc(sizeof(struct PNSYNC(_s)));
    fs->n = msequence_get_length(ms);

    fs->sym  = WINDOW(_create)(fs->n);
    fs->sym2 = WINDOW(_create)(fs->n);

    TC h[fs->n];

    unsigned int i;
    for (i=0; i<fs->n; i++)
        h[i] = msequence_advance(ms) ? 1.0f : -1.0f;

    msequence_destroy(ms);

    fs->dp = DOTPROD(_create)(h,fs->n);

    return fs;
}

void PNSYNC(_destroy)(PNSYNC() _fs)
{
    WINDOW(_destroy)(_fs->sym);
    WINDOW(_destroy)(_fs->sym2);
    DOTPROD(_destroy)(_fs->dp);
    free(_fs);
}

void PNSYNC(_print)(PNSYNC() _fs)
{

}

TO PNSYNC(_correlate)(PNSYNC() _fs, TI _sym)
{
    // push symbol into buffers
    WINDOW(_push)(_fs->sym,  _sym);
    WINDOW(_push)(_fs->sym2, ABS(_sym)*ABS(_sym));

    // compute dotprod, energy
    TI * r2;
    unsigned int i;
    WINDOW(_read)(_fs->sym2, &r2);
    float e=0.0f;
    for (i=0; i<_fs->n; i++)
        e += r2[i];
    e /= _fs->n;

    TI * r;
    WINDOW(_read)(_fs->sym, &r);
    _fs->rxy = DOTPROD(_execute)(_fs->dp, r) / (sqrtf(e) * _fs->n);

    return _fs->rxy;
}

