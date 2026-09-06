/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "liquid.autotest.h"
#include "liquid.internal.h"

// Helper function to keep code base small
void testbench_modemcf_copy(liquid_autotest __q__, modulation_scheme _ms)
{
    // create modem and randomize internal state
    modemcf modem_0 = modemcf_create(_ms);
    unsigned int i, s0, s1, M = 1 << modemcf_get_bps(modem_0);
    float complex x0, x1;
    for (i=0; i<10; i++) {
        // modulate random symbol
        modemcf_modulate(modem_0, rand() % M, &x0);

        // demodulate random sample
        modemcf_demodulate(modem_0, randnf() + _Complex_I*randnf(), &s0);
    }

    // copy modem
    modemcf modem_1 = modemcf_copy(modem_0);

    liquid_log_debug("input: %20s, 0: %20s, 1:%20s",
        modulation_types[_ms                        ].name,
        modulation_types[modemcf_get_scheme(modem_0)].name,
        modulation_types[modemcf_get_scheme(modem_1)].name);

    // ...
    for (i=0; i<10; i++) {
        // modulate random symbol
        unsigned int s = rand() % M;
        modemcf_modulate(modem_0, s, &x0);
        modemcf_modulate(modem_1, s, &x1);
        LIQUID_CHECK(x0 ==  x1);

        // demodulate random sample
        float complex x = randnf() + _Complex_I*randnf();
        modemcf_demodulate(modem_0, x, &s0);
        modemcf_demodulate(modem_1, x, &s1);
        LIQUID_CHECK(s0 ==  s1)
    }

    // clean it up
    modemcf_destroy(modem_0);
    modemcf_destroy(modem_1);
}

// AUTOTESTS: generic PSK
LIQUID_AUTOTEST(modem_copy_psk2,    "modem copy, psk2","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK2);      }
LIQUID_AUTOTEST(modem_copy_psk4,    "modem copy, psk4","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK4);      }
LIQUID_AUTOTEST(modem_copy_psk8,    "modem copy, psk8","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK8);      }
LIQUID_AUTOTEST(modem_copy_psk16,   "modem copy, psk16","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK16);     }
LIQUID_AUTOTEST(modem_copy_psk32,   "modem copy, psk32","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK32);     }
LIQUID_AUTOTEST(modem_copy_psk64,   "modem copy, psk64","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK64);     }
LIQUID_AUTOTEST(modem_copy_psk128,  "modem copy, psk128","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK128);    }
LIQUID_AUTOTEST(modem_copy_psk256,  "modem copy, psk256","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_PSK256);    }

// AUTOTESTS: generic DPSK
LIQUID_AUTOTEST(modem_copy_dpsk2,   "modem copy, dpsk2","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK2);     }
LIQUID_AUTOTEST(modem_copy_dpsk4,   "modem copy, dpsk4","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK4);     }
LIQUID_AUTOTEST(modem_copy_dpsk8,   "modem copy, dpsk8","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK8);     }
LIQUID_AUTOTEST(modem_copy_dpsk16,  "modem copy, dpsk16","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK16);    }
LIQUID_AUTOTEST(modem_copy_dpsk32,  "modem copy, dpsk32","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK32);    }
LIQUID_AUTOTEST(modem_copy_dpsk64,  "modem copy, dpsk64","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK64);    }
LIQUID_AUTOTEST(modem_copy_dpsk128, "modem copy, dpsk128","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK128);   }
LIQUID_AUTOTEST(modem_copy_dpsk256, "modem copy, dpsk256","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_DPSK256);   }

// AUTOTESTS: generic ASK
LIQUID_AUTOTEST(modem_copy_ask2,    "modem copy, ask2","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK2);      }
LIQUID_AUTOTEST(modem_copy_ask4,    "modem copy, ask4","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK4);      }
LIQUID_AUTOTEST(modem_copy_ask8,    "modem copy, ask8","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK8);      }
LIQUID_AUTOTEST(modem_copy_ask16,   "modem copy, ask16","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK16);     }
LIQUID_AUTOTEST(modem_copy_ask32,   "modem copy, ask32","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK32);     }
LIQUID_AUTOTEST(modem_copy_ask64,   "modem copy, ask64","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK64);     }
LIQUID_AUTOTEST(modem_copy_ask128,  "modem copy, ask128","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK128);    }
LIQUID_AUTOTEST(modem_copy_ask256,  "modem copy, ask256","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_ASK256);    }

// AUTOTESTS: generic QAM
LIQUID_AUTOTEST(modem_copy_qam4,    "modem copy, qam4","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM4);      }
LIQUID_AUTOTEST(modem_copy_qam8,    "modem copy, qam8","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM8);      }
LIQUID_AUTOTEST(modem_copy_qam16,   "modem copy, qam16","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM16);     }
LIQUID_AUTOTEST(modem_copy_qam32,   "modem copy, qam32","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM32);     }
LIQUID_AUTOTEST(modem_copy_qam64,   "modem copy, qam64","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM64);     }
LIQUID_AUTOTEST(modem_copy_qam128,  "modem copy, qam128","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM128);    }
LIQUID_AUTOTEST(modem_copy_qam256,  "modem copy, qam256","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_QAM256);    }

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
LIQUID_AUTOTEST(modem_copy_apsk4,   "modem copy, apsk4","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK4);     }
LIQUID_AUTOTEST(modem_copy_apsk8,   "modem copy, apsk8","modem,config",0.1)     { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK8);     }
LIQUID_AUTOTEST(modem_copy_apsk16,  "modem copy, apsk16","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK16);    }
LIQUID_AUTOTEST(modem_copy_apsk32,  "modem copy, apsk32","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK32);    }
LIQUID_AUTOTEST(modem_copy_apsk64,  "modem copy, apsk64","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK64);    }
LIQUID_AUTOTEST(modem_copy_apsk128, "modem copy, apsk128","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK128);   }
LIQUID_AUTOTEST(modem_copy_apsk256, "modem copy, apsk256","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_APSK256);   }

// AUTOTESTS: Specific modems
LIQUID_AUTOTEST(modem_copy_bpsk,    "modem copy, bpsk","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_BPSK);      }
LIQUID_AUTOTEST(modem_copy_qpsk,    "modem copy, qpsk","modem,config",0.1)      { testbench_modemcf_copy(__q__,LIQUID_MODEM_QPSK);      }
LIQUID_AUTOTEST(modem_copy_ook,     "modem copy, ook","modem,config",0.1)       { testbench_modemcf_copy(__q__,LIQUID_MODEM_OOK);       }
LIQUID_AUTOTEST(modem_copy_sqam32,  "modem copy, sqam32","modem,config",0.1)    { testbench_modemcf_copy(__q__,LIQUID_MODEM_SQAM32);    }
LIQUID_AUTOTEST(modem_copy_sqam128, "modem copy, sqam128","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_SQAM128);   }
LIQUID_AUTOTEST(modem_copy_V29,     "modem copy, V29","modem,config",0.1)       { testbench_modemcf_copy(__q__,LIQUID_MODEM_V29);       }
LIQUID_AUTOTEST(modem_copy_arb16opt,"modem copy, arb16opt","modem,config",0.1)  { testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB16OPT);  }
LIQUID_AUTOTEST(modem_copy_arb32opt,"modem copy, arb32opt","modem,config",0.1)  { testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB32OPT);  }
LIQUID_AUTOTEST(modem_copy_arb64opt,"modem copy, arb64opt","modem,config",0.1)  { testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB64OPT);  }
LIQUID_AUTOTEST(modem_copy_arb128opt,"modem copy, arb128opt","modem,config",0.1){ testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB128OPT); }
LIQUID_AUTOTEST(modem_copy_arb256opt,"modem copy, arb256opt","modem,config",0.1){ testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB256OPT); }
LIQUID_AUTOTEST(modem_copy_arb64vt, "modem copy, arb64vt","modem,config",0.1)   { testbench_modemcf_copy(__q__,LIQUID_MODEM_ARB64VT);   }
LIQUID_AUTOTEST(modem_copy_pi4dqpsk,"modem copy, pi4dqpsk","modem,config",0.1)  { testbench_modemcf_copy(__q__,LIQUID_MODEM_PI4DQPSK);  }

// test errors and invalid configuration
LIQUID_AUTOTEST(modem_config,"modem config","modem,config",0.1)
{
    _liquid_error_downgrade_enable();
    // test copying/creating invalid objects
    LIQUID_CHECK(NULL == modemcf_copy(NULL) );
    LIQUID_CHECK(NULL == modemcf_create(LIQUID_MODEM_ARB) );
    LIQUID_CHECK(NULL == modemcf_create(-1) );

    // create object and check configuration
    modemcf q = modemcf_create(LIQUID_MODEM_QAM64);
    LIQUID_CHECK( LIQUID_OK == modemcf_print(q) );

    // internal: try to initialize using invalid configuration
    LIQUID_CHECK( LIQUID_OK != modemcf_init(q,0) );
    LIQUID_CHECK( LIQUID_OK != modemcf_init(q,77) );

    // internal: try to modulate using invalid inputs
    float complex sym;
    LIQUID_CHECK( LIQUID_OK != modemcf_modulate    (q,8193,&sym) );
    LIQUID_CHECK( LIQUID_OK != modemcf_modulate_map(q,8193,&sym) );
    LIQUID_CHECK( LIQUID_OK != modemcf_demodsoft_gentab(q,227) );

    modemcf_destroy(q);
    _liquid_error_downgrade_disable();
}

