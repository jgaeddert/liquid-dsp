char __docstr__[] = "Test elliptic functions";

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

int main() {

    liquid_float_complex u = 0.7f;
    float k = 0.8f;
    unsigned int n=7;

    liquid_float_complex cd = ellip_cdf(u,k,n);
    liquid_float_complex sn = ellip_snf(u,k,n);
    printf("u   : %12.8f + j*%12.8f\n", crealf(u), cimagf(u));
    printf("k   : %12.8f\n", k);
    printf("cd  : %12.8f + j*%12.8f\n", crealf(cd), cimagf(cd));
    printf("sn  : %12.8f + j*%12.8f\n", crealf(sn), cimagf(sn));

    printf("\n");
    liquid_float_complex acd = ellip_acdf(cd,k,n);
    liquid_float_complex asn = ellip_asnf(sn,k,n);
    printf("acd : %12.8f + j*%12.8f\n", crealf(acd), cimagf(acd));
    printf("asn : %12.8f + j*%12.8f\n", crealf(asn), cimagf(asn));

    printf("done.\n");
    return 0;
}

