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

#include <stdio.h>

#include "liquid.autotest.h"
#include "liquid.h"

// Helper function to keep code base small
void testbench_modemcf_demodstats(liquid_autotest __q__,
                                  modulation_scheme _ms)
{
    // generate mod/demod
    modemcf mod   = modemcf_create(_ms);
    modemcf demod = modemcf_create(_ms);

    // run the test
    unsigned int i, s, M = 1 << modemcf_get_bps(mod);
    liquid_float_complex x;
    liquid_float_complex x_hat;    // rotated symbol
    float demodstats;
    float phi = 0.01f;

    for (i=0; i<M; i++) {
        // reset modemcf objects
        modemcf_reset(mod);
        modemcf_reset(demod);

        // modulate symbol
        modemcf_modulate(mod, i, &x);

        // ignore rare condition where modulated symbol is (0,0)
        // (e.g. APSK-8)
        if (cabsf(x) < 1e-3f) continue;

        // add phase offsets
        x_hat = x * cexpf( phi*_Complex_I);

        // demod positive phase signal, and ensure demodulator
        // maps to appropriate symbol
        modemcf_demodulate(demod, x_hat, &s);
        if (s != i)
            LIQUID_WARN("modem_test_demodstats(), output symbol does not match");

        demodstats = modemcf_get_demodulator_phase_error(demod);
        LIQUID_CHECK(demodstats > 0.0f);
    }

    // repeat with negative phase error
    for (i=0; i<M; i++) {
        // reset modemcf objects
        modemcf_reset(mod);
        modemcf_reset(demod);

        // modulate symbol
        modemcf_modulate(mod, i, &x);

        // ignore rare condition where modulated symbol is (0,0)
        // (e.g. APSK-8)
        if (cabsf(x) < 1e-3f) continue;

        // add phase offsets
        x_hat = x * cexpf(-phi*_Complex_I);

        // demod positive phase signal, and ensure demodulator
        // maps to appropriate symbol
        modemcf_demodulate(demod, x_hat, &s);
        if (s != i)
            LIQUID_WARN("modem_test_demodstats(), output symbol does not match");

        demodstats = modemcf_get_demodulator_phase_error(demod);
        LIQUID_CHECK(demodstats < 0.0f);
    }

    // clean up allocated objects up
    modemcf_destroy(mod);
    modemcf_destroy(demod);
}

// AUTOTESTS: generic PSK
LIQUID_AUTOTEST(demodstats_psk2,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK2);     }
LIQUID_AUTOTEST(demodstats_psk4,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK4);     }
LIQUID_AUTOTEST(demodstats_psk8,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK8);     }
LIQUID_AUTOTEST(demodstats_psk16,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK16);    }
LIQUID_AUTOTEST(demodstats_psk32,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK32);    }
LIQUID_AUTOTEST(demodstats_psk64,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK64);    }
LIQUID_AUTOTEST(demodstats_psk128,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK128);   }
LIQUID_AUTOTEST(demodstats_psk256,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_PSK256);   }

// AUTOTESTS: generic DPSK
LIQUID_AUTOTEST(demodstats_dpsk2,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK2);    }
LIQUID_AUTOTEST(demodstats_dpsk4,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK4);    }
LIQUID_AUTOTEST(demodstats_dpsk8,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK8);    }
LIQUID_AUTOTEST(demodstats_dpsk16,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK16);   }
LIQUID_AUTOTEST(demodstats_dpsk32,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK32);   }
LIQUID_AUTOTEST(demodstats_dpsk64,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK64);   }
LIQUID_AUTOTEST(demodstats_dpsk128,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK128);  }
LIQUID_AUTOTEST(demodstats_dpsk256,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_DPSK256);  }

// AUTOTESTS: generic ASK
LIQUID_AUTOTEST(demodstats_ask2,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK2);     }
LIQUID_AUTOTEST(demodstats_ask4,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK4);     }
LIQUID_AUTOTEST(demodstats_ask8,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK8);     }
LIQUID_AUTOTEST(demodstats_ask16,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK16);    }
LIQUID_AUTOTEST(demodstats_ask32,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK32);    }
LIQUID_AUTOTEST(demodstats_ask64,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK64);    }
LIQUID_AUTOTEST(demodstats_ask128,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK128);   }
LIQUID_AUTOTEST(demodstats_ask256,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ASK256);   }

// AUTOTESTS: generic QAM
LIQUID_AUTOTEST(demodstats_qam4,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM4);     }
LIQUID_AUTOTEST(demodstats_qam8,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM8);     }
LIQUID_AUTOTEST(demodstats_qam16,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM16);    }
LIQUID_AUTOTEST(demodstats_qam32,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM32);    }
LIQUID_AUTOTEST(demodstats_qam64,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM64);    }
LIQUID_AUTOTEST(demodstats_qam128,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM128);   }
LIQUID_AUTOTEST(demodstats_qam256,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QAM256);   }

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
LIQUID_AUTOTEST(demodstats_apsk4,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK4);    }
LIQUID_AUTOTEST(demodstats_apsk8,"","",0.1)    { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK8);    }
LIQUID_AUTOTEST(demodstats_apsk16,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK16);   }
LIQUID_AUTOTEST(demodstats_apsk32,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK32);   }
LIQUID_AUTOTEST(demodstats_apsk64,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK64);   }
LIQUID_AUTOTEST(demodstats_apsk128,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK128);  }
LIQUID_AUTOTEST(demodstats_apsk256,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_APSK256);  }

// AUTOTESTS: Specific modems
LIQUID_AUTOTEST(demodstats_bpsk,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_BPSK);     }
LIQUID_AUTOTEST(demodstats_qpsk,"","",0.1)     { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_QPSK);     }
LIQUID_AUTOTEST(demodstats_ook,"","",0.1)      { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_OOK);      }
LIQUID_AUTOTEST(demodstats_sqam32,"","",0.1)   { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_SQAM32);   }
LIQUID_AUTOTEST(demodstats_sqam128,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_SQAM128);  }
LIQUID_AUTOTEST(demodstats_V29,"","",0.1)      { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_V29);      }
LIQUID_AUTOTEST(demodstats_arb16opt,"","",0.1) { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB16OPT); }
LIQUID_AUTOTEST(demodstats_arb32opt,"","",0.1) { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB32OPT); }
LIQUID_AUTOTEST(demodstats_arb64opt,"","",0.1) { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB64OPT); }
LIQUID_AUTOTEST(demodstats_arb128opt,"","",0.1){ testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB128OPT);}
LIQUID_AUTOTEST(demodstats_arb256opt,"","",0.1){ testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB256OPT);}
LIQUID_AUTOTEST(demodstats_arb64vt,"","",0.1)  { testbench_modemcf_demodstats(__q__, LIQUID_MODEM_ARB64VT);  }

