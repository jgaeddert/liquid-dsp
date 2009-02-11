#ifndef __COMPLEMENTARY_CODES_AUTOTEST_H__
#define __COMPLEMENTARY_CODES_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: validate autocorrelation properties of
//           complementary codes
//
void autotest_ccodes() {
    // create and initialize codes
    unsigned int n=32;
    bsequence a = bsequence_create(n);
    bsequence b = bsequence_create(n);
    bsequence_create_ccodes(a, b);

    // print
    bsequence_print(a);
    bsequence_print(b);
    
    // clean up memory
    bsequence_destroy(a);
    bsequence_destroy(b);
}

#endif 

