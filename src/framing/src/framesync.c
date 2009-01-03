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
    return NULL;
}

void FRAMESYNC(_destroy)(FRAMESYNC() _fs)
{
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

    WINDOW(_read)(_fs->sym, &r);
    _fs->rxy = DOTPROD(_execute)(_fs->dp, r) / sqrtf(e);

    return _fs->rxy;
}

