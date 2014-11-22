/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

//
// modemf.c : linear modem API, floating-point (single precision)
//

#include <math.h>
#include "liquid.internal.h"

// Macro definitions
#define MODEM(name)         LIQUID_CONCAT(modem,name)
#define FREQMOD(name)       LIQUID_CONCAT(freqmod,name)
#define FREQDEM(name)       LIQUID_CONCAT(freqdem,name)

#define T                   float           /* primitive type */
#define TC                  float complex   /* primitive type (complex) */

#define PRINTVAL_T(X,F)     PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)

// common source must come first (object definition)
#include "modem_common.c"

// generic modem specifications
#include "modem_psk.c"
#include "modem_dpsk.c"
#include "modem_ask.c"
#include "modem_qam.c"
#include "modem_apsk.c"

// specific modems
#include "modem_bpsk.c"
#include "modem_qpsk.c"
#include "modem_ook.c"
#include "modem_sqam32.c"
#include "modem_sqam128.c"

// arbitary modems
#include "modem_arb.c"

// analog modems
#include "freqmod.c"
#include "freqdem.c"
