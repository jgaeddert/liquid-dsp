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

//
// soft demodulation tests
//

#include "liquid.autotest.h"
#include "liquid.h"

// Help function to keep code base small
void testbench_modemcf_demodsoft(liquid_autotest __q__, modulation_scheme _ms)
{
    // generate mod/demod
    modemcf mod   = modemcf_create(_ms);
    modemcf demod = modemcf_create(_ms);

    // 
    unsigned int bps = modemcf_get_bps(demod);

    // run the test
    unsigned int i, s, M=1<<bps;
    unsigned int sym_soft;
    LIQUID_VLA(unsigned char, soft_bits, bps);
    liquid_float_complex x;
    
    for (i=0; i<M; i++) {
        // modulate symbol
        modemcf_modulate(mod, i, &x);

        // demodulate using soft-decision
        modemcf_demodulate_soft(demod, x, &s, soft_bits);

        // check hard-decision output
        LIQUID_CHECK(s ==  i);

        // check soft bits
        liquid_pack_soft_bits(soft_bits, bps, &sym_soft);
        LIQUID_CHECK(sym_soft ==  i);

        // check phase error, evm, etc.
        //LIQUID_CHECK_DELTA( modemcf_get_demodulator_phase_error(demod), 0.0f, 1e-3f);
        //LIQUID_CHECK_DELTA( modemcf_get_demodulator_evm(demod), 0.0f, 1e-3f);
    }
    // clean it up
    modemcf_destroy(mod);
    modemcf_destroy(demod);
}

// AUTOTESTS: generic PSK
LIQUID_AUTOTEST(demodsoft_psk2,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK2);      }
LIQUID_AUTOTEST(demodsoft_psk4,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK4);      }
LIQUID_AUTOTEST(demodsoft_psk8,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK8);      }
LIQUID_AUTOTEST(demodsoft_psk16,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK16);     }
LIQUID_AUTOTEST(demodsoft_psk32,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK32);     }
LIQUID_AUTOTEST(demodsoft_psk64,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK64);     }
LIQUID_AUTOTEST(demodsoft_psk128,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK128);    }
LIQUID_AUTOTEST(demodsoft_psk256,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK256);    }

// AUTOTESTS: generic DPSK
LIQUID_AUTOTEST(demodsoft_dpsk2,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK2);     }
LIQUID_AUTOTEST(demodsoft_dpsk4,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK4);     }
LIQUID_AUTOTEST(demodsoft_dpsk8,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK8);     }
LIQUID_AUTOTEST(demodsoft_dpsk16,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK16);    }
LIQUID_AUTOTEST(demodsoft_dpsk32,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK32);    }
LIQUID_AUTOTEST(demodsoft_dpsk64,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK64);    }
LIQUID_AUTOTEST(demodsoft_dpsk128,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK128);   }
LIQUID_AUTOTEST(demodsoft_dpsk256,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK256);   }

// AUTOTESTS: generic ASK
LIQUID_AUTOTEST(demodsoft_ask2,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK2);      }
LIQUID_AUTOTEST(demodsoft_ask4,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK4);      }
LIQUID_AUTOTEST(demodsoft_ask8,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK8);      }
LIQUID_AUTOTEST(demodsoft_ask16,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK16);     }
LIQUID_AUTOTEST(demodsoft_ask32,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK32);     }
LIQUID_AUTOTEST(demodsoft_ask64,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK64);     }
LIQUID_AUTOTEST(demodsoft_ask128,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK128);    }
LIQUID_AUTOTEST(demodsoft_ask256,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK256);    }

// AUTOTESTS: generic QAM
LIQUID_AUTOTEST(demodsoft_qam4,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM4);      }
LIQUID_AUTOTEST(demodsoft_qam8,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM8);      }
LIQUID_AUTOTEST(demodsoft_qam16,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM16);     }
LIQUID_AUTOTEST(demodsoft_qam32,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM32);     }
LIQUID_AUTOTEST(demodsoft_qam64,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM64);     }
LIQUID_AUTOTEST(demodsoft_qam128,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM128);    }
LIQUID_AUTOTEST(demodsoft_qam256,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM256);    }

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
LIQUID_AUTOTEST(demodsoft_apsk4,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK4);     }
LIQUID_AUTOTEST(demodsoft_apsk8,"","",0.1)     { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK8);     }
LIQUID_AUTOTEST(demodsoft_apsk16,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK16);    }
LIQUID_AUTOTEST(demodsoft_apsk32,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK32);    }
LIQUID_AUTOTEST(demodsoft_apsk64,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK64);    }
LIQUID_AUTOTEST(demodsoft_apsk128,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK128);   }
LIQUID_AUTOTEST(demodsoft_apsk256,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK256);   }

// AUTOTESTS: Specific modems
LIQUID_AUTOTEST(demodsoft_bpsk,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_BPSK);      }
LIQUID_AUTOTEST(demodsoft_qpsk,"","",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QPSK);      }
LIQUID_AUTOTEST(demodsoft_ook,"","",0.1)       { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_OOK);       }
LIQUID_AUTOTEST(demodsoft_sqam32,"","",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_SQAM32);    }
LIQUID_AUTOTEST(demodsoft_sqam128,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_SQAM128);   }
LIQUID_AUTOTEST(demodsoft_V29,"","",0.1)       { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_V29);       }
LIQUID_AUTOTEST(demodsoft_arb16opt,"","",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB16OPT);  }
LIQUID_AUTOTEST(demodsoft_arb32opt,"","",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB32OPT);  }
LIQUID_AUTOTEST(demodsoft_arb64opt,"","",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB64OPT);  }
LIQUID_AUTOTEST(demodsoft_arb128opt,"","",0.1) { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB128OPT); }
LIQUID_AUTOTEST(demodsoft_arb256opt,"","",0.1) { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB256OPT); }
LIQUID_AUTOTEST(demodsoft_arb64vt,"","",0.1)   { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB64VT);   }
LIQUID_AUTOTEST(demodsoft_pi4dqpsk,"","",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PI4DQPSK);  }

