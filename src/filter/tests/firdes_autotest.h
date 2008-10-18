#ifndef __FIRDES_AUTOTEST_H__
#define __FIRDES_AUTOTEST_H__

#include "../src/firdes.h"
#include "../src/rrcos.h"

void autotest_design_rrcos_filter() {

    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;

    // Initialize pre-determined coefficient array
    float h0[12] = {
        -0.0331158, 0.0450158,  0.0565969, -0.1536039, -0.0750015, 0.6153450,
         1.0819719, 0.6153450, -0.0750015, -0.1536039,  0.0565969, 0.0450158
    };

    // Create filter
    float h[12];
    design_rrc_filter(k,m,beta,offset,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<12; i++)
        CONTEND_DELTA( h[i], h0[i], 0.00001f );
}


#endif // __FIRDES_AUTOTEST_H__

