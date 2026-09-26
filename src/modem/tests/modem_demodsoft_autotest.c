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
    unsigned char soft_bits[bps];
    float complex x;
    
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
LIQUID_AUTOTEST(modem_demodsoft_psk2,"modem soft demod, psk2","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK2);      }
LIQUID_AUTOTEST(modem_demodsoft_psk4,"modem soft demod, psk4","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK4);      }
LIQUID_AUTOTEST(modem_demodsoft_psk8,"modem soft demod, psk8","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK8);      }
LIQUID_AUTOTEST(modem_demodsoft_psk16,"modem soft demod, psk16","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK16);     }
LIQUID_AUTOTEST(modem_demodsoft_psk32,"modem soft demod, psk32","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK32);     }
LIQUID_AUTOTEST(modem_demodsoft_psk64,"modem soft demod, psk64","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK64);     }
LIQUID_AUTOTEST(modem_demodsoft_psk128,"modem soft demod, psk128","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK128);    }
LIQUID_AUTOTEST(modem_demodsoft_psk256,"modem soft demod, psk256","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PSK256);    }

// AUTOTESTS: generic DPSK
LIQUID_AUTOTEST(modem_demodsoft_dpsk2,"modem soft demod, dpsk2","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK2);     }
LIQUID_AUTOTEST(modem_demodsoft_dpsk4,"modem soft demod, dpsk4","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK4);     }
LIQUID_AUTOTEST(modem_demodsoft_dpsk8,"modem soft demod, dpsk8","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK8);     }
LIQUID_AUTOTEST(modem_demodsoft_dpsk16,"modem soft demod, dpsk16","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK16);    }
LIQUID_AUTOTEST(modem_demodsoft_dpsk32,"modem soft demod, dpsk32","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK32);    }
LIQUID_AUTOTEST(modem_demodsoft_dpsk64,"modem soft demod, dpsk64","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK64);    }
LIQUID_AUTOTEST(modem_demodsoft_dpsk128,"modem soft demod, dpsk128","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK128);   }
LIQUID_AUTOTEST(modem_demodsoft_dpsk256,"modem soft demod, dpsk256","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_DPSK256);   }

// AUTOTESTS: generic ASK
LIQUID_AUTOTEST(modem_demodsoft_ask2,"modem soft demod, ask2","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK2);      }
LIQUID_AUTOTEST(modem_demodsoft_ask4,"modem soft demod, ask4","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK4);      }
LIQUID_AUTOTEST(modem_demodsoft_ask8,"modem soft demod, ask8","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK8);      }
LIQUID_AUTOTEST(modem_demodsoft_ask16,"modem soft demod, ask16","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK16);     }
LIQUID_AUTOTEST(modem_demodsoft_ask32,"modem soft demod, ask32","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK32);     }
LIQUID_AUTOTEST(modem_demodsoft_ask64,"modem soft demod, ask64","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK64);     }
LIQUID_AUTOTEST(modem_demodsoft_ask128,"modem soft demod, ask128","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK128);    }
LIQUID_AUTOTEST(modem_demodsoft_ask256,"modem soft demod, ask256","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ASK256);    }

// AUTOTESTS: generic QAM
LIQUID_AUTOTEST(modem_demodsoft_qam4,"modem soft demod, qam4","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM4);      }
LIQUID_AUTOTEST(modem_demodsoft_qam8,"modem soft demod, qam8","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM8);      }
LIQUID_AUTOTEST(modem_demodsoft_qam16,"modem soft demod, qam16","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM16);     }
LIQUID_AUTOTEST(modem_demodsoft_qam32,"modem soft demod, qam32","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM32);     }
LIQUID_AUTOTEST(modem_demodsoft_qam64,"modem soft demod, qam64","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM64);     }
LIQUID_AUTOTEST(modem_demodsoft_qam128,"modem soft demod, qam128","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM128);    }
LIQUID_AUTOTEST(modem_demodsoft_qam256,"modem soft demod, qam256","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QAM256);    }

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
LIQUID_AUTOTEST(modem_demodsoft_apsk4,"modem soft demod, apsk4","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK4);     }
LIQUID_AUTOTEST(modem_demodsoft_apsk8,"modem soft demod, apsk8","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK8);     }
LIQUID_AUTOTEST(modem_demodsoft_apsk16,"modem soft demod, apsk16","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK16);    }
LIQUID_AUTOTEST(modem_demodsoft_apsk32,"modem soft demod, apsk32","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK32);    }
LIQUID_AUTOTEST(modem_demodsoft_apsk64,"modem soft demod, apsk64","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK64);    }
LIQUID_AUTOTEST(modem_demodsoft_apsk128,"modem soft demod, apsk128","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK128);   }
LIQUID_AUTOTEST(modem_demodsoft_apsk256,"modem soft demod, apsk256","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_APSK256);   }

// AUTOTESTS: Specific modems
LIQUID_AUTOTEST(modem_demodsoft_bpsk,"modem soft demod, bpsk","modem,demodsoft",0.1)            { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_BPSK);      }
LIQUID_AUTOTEST(modem_demodsoft_qpsk,"modem soft demod, qpsk","modem,demodsoft",0.1)            { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_QPSK);      }
LIQUID_AUTOTEST(modem_demodsoft_ook,"modem soft demod, ook","modem,demodsoft",0.1)              { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_OOK);       }
LIQUID_AUTOTEST(modem_demodsoft_sqam32,"modem soft demod, sqam32","modem,demodsoft",0.1)        { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_SQAM32);    }
LIQUID_AUTOTEST(modem_demodsoft_sqam128,"modem soft demod, sqam128","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_SQAM128);   }
LIQUID_AUTOTEST(modem_demodsoft_V29,"modem soft demod, V29","modem,demodsoft",0.1)              { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_V29);       }
LIQUID_AUTOTEST(modem_demodsoft_arb16opt,"modem soft demod, arb16opt","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB16OPT);  }
LIQUID_AUTOTEST(modem_demodsoft_arb32opt,"modem soft demod, arb32opt","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB32OPT);  }
LIQUID_AUTOTEST(modem_demodsoft_arb64opt,"modem soft demod, arb64opt","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB64OPT);  }
LIQUID_AUTOTEST(modem_demodsoft_arb128opt,"modem soft demod, arb128opt","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB128OPT); }
LIQUID_AUTOTEST(modem_demodsoft_arb256opt,"modem soft demod, arb256opt","modem,demodsoft",0.1)  { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB256OPT); }
LIQUID_AUTOTEST(modem_demodsoft_arb64vt,"modem soft demod, arb64vt","modem,demodsoft",0.1)      { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_ARB64VT);   }
LIQUID_AUTOTEST(modem_demodsoft_pi4dqpsk,"modem soft demod, pi4dqpsk","modem,demodsoft",0.1)    { testbench_modemcf_demodsoft(__q__, LIQUID_MODEM_PI4DQPSK);  }

