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
// autotest fft data for 17-point transform
//

#include <complex.h>

float complex fft_test_x17[] = {
   -0.655587921776 +  -0.863886550358*_Complex_I,
   -1.070139919529 +  -2.370656605854*_Complex_I,
    1.167384281127 +   0.116747569575*_Complex_I,
    1.105640858428 +  -1.226141009276*_Complex_I,
   -0.349562744440 +  -0.005953723423*_Complex_I,
    0.655312446998 +   1.594642817084*_Complex_I,
   -0.230495501721 +  -0.272806523691*_Complex_I,
    0.731764183318 +  -0.732380509571*_Complex_I,
   -0.953609619255 +   1.102615917395*_Complex_I,
   -0.496898865182 +  -1.822920499315*_Complex_I,
   -2.052948327746 +  -0.382334768518*_Complex_I,
    1.062783811112 +   0.673250838256*_Complex_I,
   -0.142175503751 +   0.542432325176*_Complex_I,
    1.108087524611 +  -0.029888173999*_Complex_I,
   -0.164259605567 +  -1.184782100304*_Complex_I,
   -0.236452649938 +  -0.998080495482*_Complex_I,
   -0.197908967423 +  -0.229518503722*_Complex_I};

float complex fft_test_y17[] = {
   -0.719066520735 +  -6.089659996026*_Complex_I,
    1.821427336790 +  -5.621144218242*_Complex_I,
   -6.173678834923 +  -4.525807530343*_Complex_I,
   -1.549626337288 +   0.314087328771*_Complex_I,
   -5.136370472539 +   4.327372264632*_Complex_I,
   -0.105170126419 +  -2.207231016009*_Complex_I,
   -5.750228396460 +   3.094152945701*_Complex_I,
    0.618725077794 +   3.278602823446*_Complex_I,
   -0.888646366224 +  -3.887677672776*_Complex_I,
    3.905822192802 +   7.933139073811*_Complex_I,
   -1.522365359136 +   0.172605279098*_Complex_I,
    8.552784050577 +  -3.105361200836*_Complex_I,
   -1.598265822441 +  -5.532288727983*_Complex_I,
    1.151099028128 +  -2.046469788513*_Complex_I,
    0.253410312032 +   6.386316111258*_Complex_I,
   -4.676752932337 +  -4.421634395132*_Complex_I,
    0.671908500186 +  -2.755072636947*_Complex_I};

