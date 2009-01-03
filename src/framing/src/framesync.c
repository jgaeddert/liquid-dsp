//
// Frame synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "framing.h"
#include "../../buffer/src/buffer.h"        // WINDOW()
#include "../../dotprod/src/dotprod.h"      // DOTPROD()
#include "../../sequence/src/sequence.h"    // p/n sequence

struct FRAMESYNC(_s) {
    unsigned int n; // header length
    WINDOW() sym;   // received symbols
    WINDOW() sym2;  // received symbols squared
    DOTPROD() dp;   // dot product
    T rxy;          // cross correlation
};

FRAMESYNC() FRAMESYNC(_create)(unsigned int _n)
{
    FRAMESYNC() fs = (FRAMESYNC()) malloc(sizeof(struct FRAMESYNC(_s)));
    //fs->n = _n;
    fs->n = 63;

    fs->sym  = WINDOW(_create)(fs->n);
    fs->sym2 = WINDOW(_create)(fs->n);

    float h[fs->n];
    msequence ms = msequence_create(6);
    unsigned int i;
    for (i=0; i<fs->n; i++)
        h[i] = msequence_advance(ms) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    fs->dp = DOTPROD(_create)(h,fs->n);

    return fs;
}

void FRAMESYNC(_destroy)(FRAMESYNC() _fs)
{
    WINDOW(_destroy)(_fs->sym);
    WINDOW(_destroy)(_fs->sym2);
    DOTPROD(_destroy)(_fs->dp);
    free(_fs);
}

void FRAMESYNC(_print)(FRAMESYNC() _fs)
{

}

T FRAMESYNC(_correlate)(FRAMESYNC() _fs, T _sym)
{
    // push symbol into buffers
    WINDOW(_push)(_fs->sym,  _sym);
    WINDOW(_push)(_fs->sym2, _sym*_sym);

    // compute dotprod, energy
    T * r;
    unsigned int i;
    WINDOW(_read)(_fs->sym2, &r);
    float e=0.0f;
    for (i=0; i<_fs->n; i++)
        e += r[i];
    e /= _fs->n;

    WINDOW(_read)(_fs->sym, &r);
    _fs->rxy = DOTPROD(_execute)(_fs->dp, r) / (sqrtf(e) * _fs->n);

    return _fs->rxy;
}

