/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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

#include "liquid.internal.h"

// Macro definitions
#define MODEM(name)         LIQUID_CONCAT(modem,name)

#define T                   float           /* primitive type */
#define TC                  float complex   /* primitive type (complex) */

#define PRINTVAL_T(X,F)     PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)

// include main files
#include "modem_common.c"           // common source must come first (object definition)
#include "modem_apsk_const.c"       // 
#include "modem_arb_const.c"        // 
#include "modem_create.c"           // 
#include "modem_demod_soft_const.c" // 
#include "modem_demodulate.c"       // 
#include "modem_demodulate_soft.c"  // 
#include "modem_modulate.c"         // 
#include "modem_arb16opt_const.c"   // 
#include "modem_arb32opt_const.c"   // 
#include "modem_arb64opt_const.c"   // 
#include "modem_arb128opt_const.c"  // 
#include "modem_arb256opt_const.c"  // 

