#ifndef __DECIM_AUTOTEST_H__
#define __DECIM_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/decim_internal.h"


//
// AUTOTEST: 
//
void autotest_decim_generic()
{
    decim d = decim_create(2, 0.0f, 0.1f, 40.0f);

    if (_autotest_verbose)
        decim_debug_print(d);

    CONTEND_LESS_THAN(d->h_len,FIR_FILTER_LEN_MAX+1);
    CONTEND_EQUALITY(d->fc, 0.0f);

    decim_destroy(d);
}

#endif 

