/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// ofdmframe64 physical layer convergence procedure (PLCP) data
//

#include "liquid.experimental.h"
#include "liquid.internal.h"

int ofdmframe64_getsctype(unsigned int _id)
{
    if (_id==0 || (_id>26 && _id<38))
        return OFDMFRAME64_SCTYPE_NULL;
    else if (_id==11 || _id==25 || _id==39 || _id==53)
        return OFDMFRAME64_SCTYPE_PILOT;
    else
        return OFDMFRAME64_SCTYPE_DATA;
}

// PLCP short sequence (frequency domain)
const float complex ofdmframe64_plcp_Sf[64] = {
      0.000000+  0.000000*_Complex_I,   0.0f, 0.0f, 0.0f,
     -0.707110+ -0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
     -0.707110+ -0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.000000+  0.000000*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.000000+  0.000000*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.000000+  0.000000*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
     -0.707110+ -0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
     -0.707110+ -0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
     -0.707110+ -0.707110*_Complex_I,   0.0f, 0.0f, 0.0f,
      0.707110+  0.707110*_Complex_I,   0.0f, 0.0f, 0.0f
};

// PLCP short sequence (time domain)
const float complex ofdmframe64_plcp_St[64] = {
      0.408250+  0.408250*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
      0.816500+  0.000000*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
      0.408250+  0.408250*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
      0.000000+  0.816500*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
      0.408250+  0.408250*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
      0.816500+  0.000000*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
      0.408250+  0.408250*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
      0.000000+  0.816500*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
      0.408250+  0.408250*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
      0.816500+  0.000000*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
      0.408250+  0.408250*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
      0.000000+  0.816500*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
      0.408250+  0.408250*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
      0.816500+  0.000000*_Complex_I,   1.267000+ -0.112280*_Complex_I, 
     -0.119570+ -0.696920*_Complex_I,  -1.175500+  0.020764*_Complex_I, 
      0.408250+  0.408250*_Complex_I,   0.020764+ -1.175500*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
      0.000000+  0.816500*_Complex_I,  -0.112280+  1.267000*_Complex_I, 
     -0.696920+ -0.119570*_Complex_I,   0.020764+ -1.175500*_Complex_I
};

// PLCP long sequence (frequency domain)
const float complex ofdmframe64_plcp_Lf[64] = {
      0.00,   1.00,  -1.00,  -1.00,   1.00,   1.00,  -1.00,   1.00, 
     -1.00,   1.00,  -1.00,  -1.00,  -1.00,  -1.00,  -1.00,   1.00, 
      1.00,  -1.00,  -1.00,   1.00,  -1.00,   1.00,  -1.00,   1.00, 
      1.00,   1.00,   1.00,   0.00,   0.00,   0.00,   0.00,   0.00, 
      0.00,   0.00,   0.00,   0.00,   0.00,   0.00,   1.00,   1.00, 
     -1.00,  -1.00,   1.00,   1.00,  -1.00,   1.00,  -1.00,   1.00, 
      1.00,   1.00,   1.00,   1.00,   1.00,  -1.00,  -1.00,   1.00, 
      1.00,  -1.00,   1.00,  -1.00,   1.00,   1.00,   1.00,   1.00
};

// PLCP long sequence (time domain)
const float complex ofdmframe64_plcp_Lt[64] = {
      1.3868+  0.0000*_Complex_I,  -0.0455+ -1.0679*_Complex_I, 
      0.3528+ -0.9866*_Complex_I,   0.8594+  0.7349*_Complex_I, 
      0.1874+  0.2475*_Complex_I,   0.5310+ -0.7784*_Complex_I, 
     -1.0218+ -0.4897*_Complex_I,  -0.3401+ -0.9423*_Complex_I, 
      0.8657+ -0.2298*_Complex_I,   0.4734+  0.0362*_Complex_I, 
      0.0088+ -1.0207*_Complex_I,  -1.2142+ -0.4205*_Complex_I, 
      0.2172+ -0.5195*_Complex_I,   0.5207+ -0.1326*_Complex_I, 
     -0.1995+  1.4259*_Complex_I,   1.0583+ -0.0363*_Complex_I, 
      0.5547+ -0.5547*_Complex_I,   0.3276+  0.8728*_Complex_I, 
     -0.5077+  0.3488*_Complex_I,  -1.1650+  0.5789*_Complex_I, 
      0.7297+  0.8197*_Complex_I,   0.6173+  0.1253*_Complex_I, 
     -0.5353+  0.7214*_Complex_I,  -0.5010+ -0.1935*_Complex_I, 
     -0.3110+ -1.3392*_Complex_I,  -1.0818+ -0.1470*_Complex_I, 
     -1.1300+ -0.1820*_Complex_I,   0.6663+ -0.6571*_Complex_I, 
     -0.0249+  0.4773*_Complex_I,  -0.8155+  1.0218*_Complex_I, 
      0.8140+  0.9396*_Complex_I,   0.1090+  0.8662*_Complex_I, 
     -1.3868+  0.0000*_Complex_I,   0.1090+ -0.8662*_Complex_I, 
      0.8140+ -0.9396*_Complex_I,  -0.8155+ -1.0218*_Complex_I, 
     -0.0249+ -0.4773*_Complex_I,   0.6663+  0.6571*_Complex_I, 
     -1.1300+  0.1820*_Complex_I,  -1.0818+  0.1470*_Complex_I, 
     -0.3110+  1.3392*_Complex_I,  -0.5010+  0.1935*_Complex_I, 
     -0.5353+ -0.7214*_Complex_I,   0.6173+ -0.1253*_Complex_I, 
      0.7297+ -0.8197*_Complex_I,  -1.1650+ -0.5789*_Complex_I, 
     -0.5077+ -0.3488*_Complex_I,   0.3276+ -0.8728*_Complex_I, 
      0.5547+  0.5547*_Complex_I,   1.0583+  0.0363*_Complex_I, 
     -0.1995+ -1.4259*_Complex_I,   0.5207+  0.1326*_Complex_I, 
      0.2172+  0.5195*_Complex_I,  -1.2142+  0.4205*_Complex_I, 
      0.0088+  1.0207*_Complex_I,   0.4734+ -0.0362*_Complex_I, 
      0.8657+  0.2298*_Complex_I,  -0.3401+  0.9423*_Complex_I, 
     -1.0218+  0.4897*_Complex_I,   0.5310+  0.7784*_Complex_I, 
      0.1874+ -0.2475*_Complex_I,   0.8594+ -0.7349*_Complex_I, 
      0.3528+  0.9866*_Complex_I,  -0.0455+  1.0679*_Complex_I
};

