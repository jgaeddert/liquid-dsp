
#ifndef __LIQUID_AUTOTEST_REGISTRY_H__
#define __LIQUID_AUTOTEST_REGISTRY_H__

#include "liquid.autotest.h"

// forward declarations
extern struct liquid_autotest_s rresamp_crcf_baseline_P1_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P2_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P6_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P8_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P9_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P1_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P2_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P6_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P8_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P9_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_arkaiser_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_arkaiser_P5_Q3_s;
extern struct liquid_autotest_s rresamp_crcf_rrcos_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_rrcos_P5_Q3_s;
extern struct liquid_autotest_s rresamp_config_s;

// compile test registry
liquid_autotest liquid_autotest_registry[] =
{
    &rresamp_crcf_baseline_P1_Q5_s,
    &rresamp_crcf_baseline_P2_Q5_s,
    &rresamp_crcf_baseline_P3_Q5_s,
    &rresamp_crcf_baseline_P6_Q5_s,
    &rresamp_crcf_baseline_P8_Q5_s,
    &rresamp_crcf_baseline_P9_Q5_s,
    &rresamp_crcf_default_P1_Q5_s,
    &rresamp_crcf_default_P2_Q5_s,
    &rresamp_crcf_default_P3_Q5_s,
    &rresamp_crcf_default_P6_Q5_s,
    &rresamp_crcf_default_P8_Q5_s,
    &rresamp_crcf_default_P9_Q5_s,
    &rresamp_crcf_arkaiser_P3_Q5_s,
    &rresamp_crcf_arkaiser_P5_Q3_s,
    &rresamp_crcf_rrcos_P3_Q5_s,
    &rresamp_crcf_rrcos_P5_Q3_s,
    &rresamp_config_s,
    NULL
};

#endif // __LIQUID_AUTOTEST_REGISTRY_H__

