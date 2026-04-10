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

#include "liquid.autotest.h"
#include "liquid.h"

// Help function to keep code base small
void testbench_modemcf_mod_demod(liquid_autotest __q__,
                                 modulation_scheme _ms)
{
    // generate mod/demod
    modemcf mod   = modemcf_create(_ms);
    modemcf demod = modemcf_create(_ms);

    // run the test
    unsigned int i, s, M = 1 << modemcf_get_bps(mod);
    liquid_float_complex x;
    float e = 0.0f;
    for (i=0; i<M; i++) {
        modemcf_modulate(mod, i, &x);
        modemcf_demodulate(demod, x, &s);
        LIQUID_CHECK(s ==  i);

        LIQUID_CHECK_DELTA( modemcf_get_demodulator_phase_error(demod), 0.0f, 1e-3f);
        
        LIQUID_CHECK_DELTA( modemcf_get_demodulator_evm(demod), 0.0f, 1e-3f);

        e += crealf(x*conjf(x));
    }
    e = sqrtf(e / (float)M);

    LIQUID_CHECK_DELTA(e,1.0f,1e-3f);

    // clean it up
    modemcf_destroy(mod);
    modemcf_destroy(demod);
}

// AUTOTESTS: generic PSK
LIQUID_AUTOTEST(mod_demod_psk2,      "psk2",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK2);      }
LIQUID_AUTOTEST(mod_demod_psk4,      "psk4",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK4);      }
LIQUID_AUTOTEST(mod_demod_psk8,      "psk8",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK8);      }
LIQUID_AUTOTEST(mod_demod_psk16,     "psk16",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK16);     }
LIQUID_AUTOTEST(mod_demod_psk32,     "psk32",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK32);     }
LIQUID_AUTOTEST(mod_demod_psk64,     "psk64",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK64);     }
LIQUID_AUTOTEST(mod_demod_psk128,    "psk128",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK128);    }
LIQUID_AUTOTEST(mod_demod_psk256,    "psk256",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PSK256);    }

// AUTOTESTS: generic DPSK
LIQUID_AUTOTEST(mod_demod_dpsk2,     "dpsk2",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK2);     }
LIQUID_AUTOTEST(mod_demod_dpsk4,     "dpsk4",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK4);     }
LIQUID_AUTOTEST(mod_demod_dpsk8,     "dpsk8",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK8);     }
LIQUID_AUTOTEST(mod_demod_dpsk16,    "dpsk16",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK16);    }
LIQUID_AUTOTEST(mod_demod_dpsk32,    "dpsk32",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK32);    }
LIQUID_AUTOTEST(mod_demod_dpsk64,    "dpsk64",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK64);    }
LIQUID_AUTOTEST(mod_demod_dpsk128,   "dpsk128",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK128);   }
LIQUID_AUTOTEST(mod_demod_dpsk256,   "dpsk256",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_DPSK256);   }

// AUTOTESTS: generic ASK
LIQUID_AUTOTEST(mod_demod_ask2,      "ask2",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK2);      }
LIQUID_AUTOTEST(mod_demod_ask4,      "ask4",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK4);      }
LIQUID_AUTOTEST(mod_demod_ask8,      "ask8",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK8);      }
LIQUID_AUTOTEST(mod_demod_ask16,     "ask16",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK16);     }
LIQUID_AUTOTEST(mod_demod_ask32,     "ask32",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK32);     }
LIQUID_AUTOTEST(mod_demod_ask64,     "ask64",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK64);     }
LIQUID_AUTOTEST(mod_demod_ask128,    "ask128",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK128);    }
LIQUID_AUTOTEST(mod_demod_ask256,    "ask256",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ASK256);    }

// AUTOTESTS: generic QAM
LIQUID_AUTOTEST(mod_demod_qam4,      "qam4",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM4);      }
LIQUID_AUTOTEST(mod_demod_qam8,      "qam8",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM8);      }
LIQUID_AUTOTEST(mod_demod_qam16,     "qam16",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM16);     }
LIQUID_AUTOTEST(mod_demod_qam32,     "qam32",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM32);     }
LIQUID_AUTOTEST(mod_demod_qam64,     "qam64",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM64);     }
LIQUID_AUTOTEST(mod_demod_qam128,    "qam128",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM128);    }
LIQUID_AUTOTEST(mod_demod_qam256,    "qam256",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QAM256);    }

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
LIQUID_AUTOTEST(mod_demod_apsk4,     "apsk4",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK4);     }
LIQUID_AUTOTEST(mod_demod_apsk8,     "apsk8",     "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK8);     }
LIQUID_AUTOTEST(mod_demod_apsk16,    "apsk16",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK16);    }
LIQUID_AUTOTEST(mod_demod_apsk32,    "apsk32",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK32);    }
LIQUID_AUTOTEST(mod_demod_apsk64,    "apsk64",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK64);    }
LIQUID_AUTOTEST(mod_demod_apsk128,   "apsk128",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK128);   }
LIQUID_AUTOTEST(mod_demod_apsk256,   "apsk256",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_APSK256);   }

// AUTOTESTS: Specific modems
LIQUID_AUTOTEST(mod_demod_bpsk,      "bpsk",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_BPSK);      }
LIQUID_AUTOTEST(mod_demod_qpsk,      "qpsk",      "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_QPSK);      }
LIQUID_AUTOTEST(mod_demod_ook,       "ook",       "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_OOK);       }
LIQUID_AUTOTEST(mod_demod_sqam32,    "sqam32",    "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_SQAM32);    }
LIQUID_AUTOTEST(mod_demod_sqam128,   "sqam128",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_SQAM128);   }
LIQUID_AUTOTEST(mod_demod_V29,       "V29",       "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_V29);       }
LIQUID_AUTOTEST(mod_demod_arb16opt,  "arb16opt",  "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB16OPT);  }
LIQUID_AUTOTEST(mod_demod_arb32opt,  "arb32opt",  "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB32OPT);  }
LIQUID_AUTOTEST(mod_demod_arb64opt,  "arb64opt",  "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB64OPT);  }
LIQUID_AUTOTEST(mod_demod_arb128opt, "arb128opt", "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB128OPT); }
LIQUID_AUTOTEST(mod_demod_arb256opt, "arb256opt", "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB256OPT); }
LIQUID_AUTOTEST(mod_demod_arb64vt,   "arb64vt",   "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_ARB64VT);   }
LIQUID_AUTOTEST(mod_demod_pi4dqpsk,  "pi4dqpsk",  "modem",0.1) { testbench_modemcf_mod_demod(__q__, LIQUID_MODEM_PI4DQPSK);  }

