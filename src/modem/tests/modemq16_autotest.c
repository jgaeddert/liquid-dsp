/*
 * Copyright (c) 2007, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2012 Virginia Polytechnic
 *                                  Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autotest/autotest.h"
#include "liquid.h"

// Help function to keep code base small
void modemq16_test_mod_demod(modulation_scheme _ms)
{
    float tol = q16_float_to_fixed(0.01f);

    // generate mod/demod
    modemq16 mod   = modemq16_create(_ms);
    modemq16 demod = modemq16_create(_ms);

    // run the test
    unsigned int i;
    unsigned int s;
    unsigned int M = 1 << modemq16_get_bps(mod);
    cq16_t x;
    float e = 0.0f;
    for (i=0; i<M; i++) {
        modemq16_modulate(mod, i, &x);
        modemq16_demodulate(demod, x, &s);
        CONTEND_EQUALITY(s, i);

        CONTEND_DELTA( modemq16_get_demodulator_phase_error(demod), 0, tol);
        
        CONTEND_DELTA( modemq16_get_demodulator_evm(demod), 0, tol);

        float complex xf = cq16_fixed_to_float(x);
        e += crealf(xf*conjf(xf));
    }
    e = sqrtf(e / (float)M);

    CONTEND_DELTA(e,1.0f,tol);

    // clean it up
    modemq16_destroy(mod);
    modemq16_destroy(demod);
}

// AUTOTESTS: generic PSK
#if 0
void xautotest_modemq16_psk2()      { modemq16_test_mod_demod(LIQUID_MODEM_PSK2);      }
void xautotest_modemq16_psk4()      { modemq16_test_mod_demod(LIQUID_MODEM_PSK4);      }
void xautotest_modemq16_psk8()      { modemq16_test_mod_demod(LIQUID_MODEM_PSK8);      }
void xautotest_modemq16_psk16()     { modemq16_test_mod_demod(LIQUID_MODEM_PSK16);     }
void xautotest_modemq16_psk32()     { modemq16_test_mod_demod(LIQUID_MODEM_PSK32);     }
void xautotest_modemq16_psk64()     { modemq16_test_mod_demod(LIQUID_MODEM_PSK64);     }
void xautotest_modemq16_psk128()    { modemq16_test_mod_demod(LIQUID_MODEM_PSK128);    }
void xautotest_modemq16_psk256()    { modemq16_test_mod_demod(LIQUID_MODEM_PSK256);    }
#endif

// AUTOTESTS: generic DPSK
#if 0
void xautotest_modemq16_dpsk2()     { modemq16_test_mod_demod(LIQUID_MODEM_DPSK2);     }
void xautotest_modemq16_dpsk4()     { modemq16_test_mod_demod(LIQUID_MODEM_DPSK4);     }
void xautotest_modemq16_dpsk8()     { modemq16_test_mod_demod(LIQUID_MODEM_DPSK8);     }
void xautotest_modemq16_dpsk16()    { modemq16_test_mod_demod(LIQUID_MODEM_DPSK16);    }
void xautotest_modemq16_dpsk32()    { modemq16_test_mod_demod(LIQUID_MODEM_DPSK32);    }
void xautotest_modemq16_dpsk64()    { modemq16_test_mod_demod(LIQUID_MODEM_DPSK64);    }
void xautotest_modemq16_dpsk128()   { modemq16_test_mod_demod(LIQUID_MODEM_DPSK128);   }
void xautotest_modemq16_dpsk256()   { modemq16_test_mod_demod(LIQUID_MODEM_DPSK256);   }
#endif

// AUTOTESTS: generic ASK
void autotest_modemq16_ask2()      { modemq16_test_mod_demod(LIQUID_MODEM_ASK2);      }
void autotest_modemq16_ask4()      { modemq16_test_mod_demod(LIQUID_MODEM_ASK4);      }
void autotest_modemq16_ask8()      { modemq16_test_mod_demod(LIQUID_MODEM_ASK8);      }
void autotest_modemq16_ask16()     { modemq16_test_mod_demod(LIQUID_MODEM_ASK16);     }
void autotest_modemq16_ask32()     { modemq16_test_mod_demod(LIQUID_MODEM_ASK32);     }
void autotest_modemq16_ask64()     { modemq16_test_mod_demod(LIQUID_MODEM_ASK64);     }
void autotest_modemq16_ask128()    { modemq16_test_mod_demod(LIQUID_MODEM_ASK128);    }
void autotest_modemq16_ask256()    { modemq16_test_mod_demod(LIQUID_MODEM_ASK256);    }

// AUTOTESTS: generic QAM
#if 0
void xautotest_modemq16_qam4()      { modemq16_test_mod_demod(LIQUID_MODEM_QAM4);      }
void xautotest_modemq16_qam8()      { modemq16_test_mod_demod(LIQUID_MODEM_QAM8);      }
void xautotest_modemq16_qam16()     { modemq16_test_mod_demod(LIQUID_MODEM_QAM16);     }
void xautotest_modemq16_qam32()     { modemq16_test_mod_demod(LIQUID_MODEM_QAM32);     }
void xautotest_modemq16_qam64()     { modemq16_test_mod_demod(LIQUID_MODEM_QAM64);     }
void xautotest_modemq16_qam128()    { modemq16_test_mod_demod(LIQUID_MODEM_QAM128);    }
void xautotest_modemq16_qam256()    { modemq16_test_mod_demod(LIQUID_MODEM_QAM256);    }
#endif

// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
#if 0
void xautotest_modemq16_apsk4()     { modemq16_test_mod_demod(LIQUID_MODEM_APSK4);     }
void xautotest_modemq16_apsk8()     { modemq16_test_mod_demod(LIQUID_MODEM_APSK8);     }
void xautotest_modemq16_apsk16()    { modemq16_test_mod_demod(LIQUID_MODEM_APSK16);    }
void xautotest_modemq16_apsk32()    { modemq16_test_mod_demod(LIQUID_MODEM_APSK32);    }
void xautotest_modemq16_apsk64()    { modemq16_test_mod_demod(LIQUID_MODEM_APSK64);    }
void xautotest_modemq16_apsk128()   { modemq16_test_mod_demod(LIQUID_MODEM_APSK128);   }
void xautotest_modemq16_apsk256()   { modemq16_test_mod_demod(LIQUID_MODEM_APSK256);   }
#endif

// AUTOTESTS: Specific modems
#if 0
void xautotest_modemq16_bpsk()      { modemq16_test_mod_demod(LIQUID_MODEM_BPSK);      }
void xautotest_modemq16_qpsk()      { modemq16_test_mod_demod(LIQUID_MODEM_QPSK);      }
void xautotest_modemq16_ook()       { modemq16_test_mod_demod(LIQUID_MODEM_OOK);       }
void xautotest_modemq16_sqam32()    { modemq16_test_mod_demod(LIQUID_MODEM_SQAM32);    }
void xautotest_modemq16_sqam128()   { modemq16_test_mod_demod(LIQUID_MODEM_SQAM128);   }
void xautotest_modemq16_V29()       { modemq16_test_mod_demod(LIQUID_MODEM_V29);       }
void xautotest_modemq16_arb16opt()  { modemq16_test_mod_demod(LIQUID_MODEM_ARB16OPT);  }
void xautotest_modemq16_arb32opt()  { modemq16_test_mod_demod(LIQUID_MODEM_ARB32OPT);  }
void xautotest_modemq16_arb64opt()  { modemq16_test_mod_demod(LIQUID_MODEM_ARB64OPT);  }
void xautotest_modemq16_arb128opt() { modemq16_test_mod_demod(LIQUID_MODEM_ARB128OPT); }
void xautotest_modemq16_arb256opt() { modemq16_test_mod_demod(LIQUID_MODEM_ARB256OPT); }
void xautotest_modemq16_arb64vt()   { modemq16_test_mod_demod(LIQUID_MODEM_ARB64VT);   }
#endif

