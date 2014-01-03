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
// autotest fft data for 21-point transform
//

#include <complex.h>

float complex fft_test_x21[] = {
   -0.292459060095 +  -0.918931582987*_Complex_I,
   -1.517033866291 +  -0.323416256164*_Complex_I,
   -1.482752904272 +   0.244611310478*_Complex_I,
    0.319915998225 +  -0.315947516079*_Complex_I,
   -0.304653265396 +   1.887596128705*_Complex_I,
    0.411043591735 +  -0.374178478358*_Complex_I,
    0.462233294747 +   0.181705549666*_Complex_I,
   -0.053584030277 +  -1.645170622140*_Complex_I,
   -0.503521485275 +   0.734911625473*_Complex_I,
   -0.425967471739 +   0.316306288806*_Complex_I,
    0.824866046500 +  -0.830071277629*_Complex_I,
    1.806337286696 +  -0.766132673126*_Complex_I,
    0.620721751355 +   0.965491952308*_Complex_I,
   -2.440640280723 +  -0.836336433610*_Complex_I,
   -0.620347474921 +  -1.385103078296*_Complex_I,
   -0.195316065403 +   0.051333260777*_Complex_I,
   -0.448853815719 +   1.473820485114*_Complex_I,
    0.468718174491 +  -0.133593056210*_Complex_I,
   -0.912579674279 +   0.093239456860*_Complex_I,
   -0.060107647935 +  -1.182219308121*_Complex_I,
   -1.630393758159 +  -1.763218511036*_Complex_I};

float complex fft_test_y21[] = {
   -5.974374656733 +  -4.525302735571*_Complex_I,
   -3.602445825195 +  -3.324518378407*_Complex_I,
    1.072414517043 +  -2.680697729601*_Complex_I,
   -2.544321267300 +  -3.718686381182*_Complex_I,
    7.073007677928 +  -0.865187973747*_Complex_I,
   -4.293690988707 +   8.225140969655*_Complex_I,
    6.571053045547 +  -6.857090522238*_Complex_I,
    4.072721192387 +   4.943421889232*_Complex_I,
    0.156147391740 +  -4.066681194313*_Complex_I,
   -0.307481517626 +  -5.479113972476*_Complex_I,
   -3.514186056563 +   2.187768615743*_Complex_I,
    6.761786041122 +   2.434424093260*_Complex_I,
    6.161936098710 +   2.932845658178*_Complex_I,
   -0.983132976608 +   2.475188613552*_Complex_I,
    0.631299782782 +   0.701473074392*_Complex_I,
   -0.667672994573 +  -2.853889888582*_Complex_I,
   -1.447661658880 +   2.115483689466*_Complex_I,
    4.415160634353 +   1.955631737971*_Complex_I,
  -10.003872665072 +  -7.143199142087*_Complex_I,
   -2.863331203221 +  -6.794540372208*_Complex_I,
   -6.854994833120 +   1.039966706231*_Complex_I};

