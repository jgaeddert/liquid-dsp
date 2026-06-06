/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.internal.h"

// multiply two complex buffers
void autotest_vectorcf_mul_16()
{
    // error tolerance
    float tol = 4e-6;

    float complex buf_0[16] = {
      0.17702709 +   1.38978455*_Complex_I,  0.91294148 +   0.39217381*_Complex_I,
     -0.80607338 +   0.76477512*_Complex_I,  0.05099755 +  -0.87350051*_Complex_I,
      0.44513826 +  -0.49490569*_Complex_I,  0.14754967 +   2.04349962*_Complex_I,
      1.07246623 +   1.08146290*_Complex_I, -1.14028088 +   1.83380899*_Complex_I,
      0.38105361 +  -0.45591846*_Complex_I,  0.32605401 +   0.34440081*_Complex_I,
     -0.05477144 +   0.60832595*_Complex_I,  1.81667523 +  -1.12238075*_Complex_I,
     -0.87190497 +   1.10743858*_Complex_I,  1.30921403 +   1.24438643*_Complex_I,
      0.55524695 +  -1.94931519*_Complex_I, -0.87191170 +   0.91693119*_Complex_I,
    };

    float complex buf_1[16] = {
     -2.19591953 +  -0.93229692*_Complex_I,  0.17150376 +   0.56165114*_Complex_I,
      1.58354529 +  -0.50696037*_Complex_I,  1.40929619 +   0.87868803*_Complex_I,
     -0.75505072 +  -0.30867372*_Complex_I, -0.09821367 +  -0.73949106*_Complex_I,
      0.03785571 +   0.72763665*_Complex_I, -1.20262636 +  -0.88838102*_Complex_I,
      0.23323685 +   0.12456235*_Complex_I,  0.34593736 +   0.02529594*_Complex_I,
      0.33669564 +   0.39064649*_Complex_I, -2.45003867 +  -0.54862205*_Complex_I,
     -2.64870707 +   2.33444473*_Complex_I, -0.92284477 +  -2.45121397*_Complex_I,
      0.24852918 +  -0.62409860*_Complex_I, -0.87039907 +   0.90921212*_Complex_I,
    };

    // expected output
    float complex buf_2[16] = {
      0.90695465 +  -3.21689701*_Complex_I, -0.06369196 +   0.58001387*_Complex_I,
     -0.88874304 +   1.61970329*_Complex_I,  0.83940506 +  -1.18621004*_Complex_I,
     -0.48886633 +   0.23627642*_Complex_I,  1.49665833 +  -0.30981126*_Complex_I,
     -0.74631304 +   0.82130533*_Complex_I,  3.00045300 +  -1.19238329*_Complex_I,
      0.14566602 +  -0.05887206*_Complex_I,  0.10408232 +   0.12738895*_Complex_I,
     -0.25608170 +   0.18342443*_Complex_I, -5.06668711 +   1.75320816*_Complex_I,
     -0.27583337 +  -4.96869469*_Complex_I,  1.84205616 +  -4.35753918*_Complex_I,
     -1.07856989 +  -0.83099055*_Complex_I, -0.07477385 +  -1.59084868*_Complex_I,
    };

    // run vector multiplication
    float complex buf_test[16];
    liquid_vectorcf_mul(buf_0, buf_1, 16, buf_test);

    //compare result
    unsigned int i;
    for (i=0; i<16; i++) {
        CONTEND_DELTA(crealf(buf_test[i]), crealf(buf_2[i]), tol);
        CONTEND_DELTA(cimagf(buf_test[i]), cimagf(buf_2[i]), tol);
    }
}

//
void autotest_vectorcf_mul_35()
{
    float tol = 4e-6;

    float complex buf_0[35] = {
      1.11555653 +   2.30658043*_Complex_I, -0.36133676 +  -0.10917327*_Complex_I,
      0.17714505 +  -2.14631440*_Complex_I,  2.20424609 +   0.59063608*_Complex_I,
     -0.44699194 +   0.23369318*_Complex_I,  0.60613931 +   0.21868288*_Complex_I,
     -1.18746289 +  -0.52159563*_Complex_I, -0.46277775 +   0.75010157*_Complex_I,
      0.93796307 +   0.28608151*_Complex_I, -2.18699829 +   0.38029319*_Complex_I,
      0.16145611 +   0.18343353*_Complex_I, -0.62653631 +  -1.79037656*_Complex_I,
     -0.67042462 +   0.11044084*_Complex_I,  0.70333438 +   1.78729174*_Complex_I,
     -0.32923580 +   0.78514690*_Complex_I,  0.27534332 +  -0.56377431*_Complex_I,
      0.41492559 +   1.37176526*_Complex_I,  3.25368958 +   2.70495218*_Complex_I,
      1.63002035 +  -0.14193750*_Complex_I,  2.22057186 +   0.55056461*_Complex_I,
      1.40896777 +   0.80722903*_Complex_I, -0.22334033 +  -0.14227395*_Complex_I,
     -1.48631186 +   0.53610531*_Complex_I, -1.91632185 +   0.88755083*_Complex_I,
     -0.52054895 +  -0.35572001*_Complex_I, -1.56515607 +  -0.41448794*_Complex_I,
     -0.91107117 +   0.17059659*_Complex_I, -0.77007659 +   2.73381816*_Complex_I,
     -0.46645585 +   0.38994666*_Complex_I,  0.80317663 +  -0.41756968*_Complex_I,
      0.26992512 +   0.41828145*_Complex_I, -0.72456446 +   1.25002030*_Complex_I,
      1.19573306 +   0.98449546*_Complex_I,  1.42491943 +  -0.55426305*_Complex_I,
      1.08243614 +   0.35774368*_Complex_I, };

    float complex buf_1[35] = {
     -0.82466736 +  -1.39329228*_Complex_I, -1.46176052 +  -1.96218827*_Complex_I,
     -1.28388174 +  -0.07152934*_Complex_I, -0.51910014 +  -0.37915971*_Complex_I,
     -0.65964708 +  -0.98417534*_Complex_I, -1.40213479 +  -0.82198463*_Complex_I,
      0.86051446 +   0.97926463*_Complex_I,  0.26257342 +   0.76586696*_Complex_I,
      0.72174183 +  -1.89884636*_Complex_I, -0.26018863 +   1.06920599*_Complex_I,
      0.57949117 +  -0.77431546*_Complex_I,  0.84635184 +  -0.81123009*_Complex_I,
     -1.12637629 +  -0.42027412*_Complex_I, -1.04214881 +   0.90519721*_Complex_I,
      0.54458433 +  -1.03487314*_Complex_I, -0.17847893 +   2.20358978*_Complex_I,
      0.19642532 +  -0.07449796*_Complex_I, -1.84958229 +   0.13218920*_Complex_I,
     -1.49042886 +   0.81610408*_Complex_I, -0.27466940 +  -1.48438409*_Complex_I,
      0.29239375 +   0.72443343*_Complex_I, -1.20243456 +  -2.77032750*_Complex_I,
     -0.41784260 +   0.77455254*_Complex_I,  0.37737465 +  -0.52426993*_Complex_I,
     -1.25500377 +   1.76270122*_Complex_I,  1.55976056 +  -1.18189171*_Complex_I,
     -0.05111343 +  -1.18849396*_Complex_I, -1.92966664 +   0.66504899*_Complex_I,
     -2.82387897 +   1.41128242*_Complex_I, -1.48171326 +  -0.03347470*_Complex_I,
      0.38047273 +  -1.40969799*_Complex_I,  1.71995272 +   0.00298203*_Complex_I,
      0.56040910 +  -0.12713027*_Complex_I, -0.46653022 +  -0.65450499*_Complex_I,
      0.15515755 +   1.58944030*_Complex_I, };

    float complex buf_2[35] = {
      2.29377794 +  -3.45645785*_Complex_I,  0.31396931 +   0.86859596*_Complex_I,
     -0.38095775 +   2.74294305*_Complex_I, -0.92027903 +  -1.14236057*_Complex_I,
      0.52485198 +   0.28576344*_Complex_I, -0.67013502 +  -0.80486012*_Complex_I,
     -0.51104885 +  -1.61168110*_Complex_I, -0.69599110 +  -0.15746944*_Complex_I,
      1.22019207 +  -1.57457078*_Complex_I,  0.16242036 +  -2.43729973*_Complex_I,
      0.23559782 +  -0.01871984*_Complex_I, -1.98267746 +  -1.00702333*_Complex_I,
      0.80156583 +   0.15736417*_Complex_I, -2.35083055 +  -1.22596765*_Complex_I,
      0.63323075 +   0.76829600*_Complex_I,  1.19318426 +   0.70736551*_Complex_I,
      0.18369560 +   0.23853831*_Complex_I, -6.37553215 +  -4.57292891*_Complex_I,
     -2.31359363 +   1.54181397*_Complex_I,  0.20732617 +  -3.44740438*_Complex_I,
     -0.17281032 +   1.25673199*_Complex_I, -0.12559330 +   0.78980100*_Complex_I,
      0.20580271 +  -1.37523425*_Complex_I, -0.25785509 +   1.33960915*_Complex_I,
      1.28031898 +  -0.47114229*_Complex_I, -2.93114853 +   1.20334303*_Complex_I,
      0.24932098 +   1.07408285*_Complex_I, -0.33213186 +  -5.78749657*_Complex_I,
      0.76688999 +  -1.75946307*_Complex_I, -1.20405555 +   0.59183240*_Complex_I,
      0.69234967 +  -0.22136822*_Complex_I, -1.24994409 +   2.14781499*_Complex_I,
      0.79525888 +   0.39970639*_Complex_I, -1.02753592 +  -0.67403650*_Complex_I,
     -0.40066409 +   1.77597415*_Complex_I, };

    // run vector multiplication
    float complex buf_test[35];
    liquid_vectorcf_mul(buf_0, buf_1, 35, buf_test);

    //compare result
    unsigned int i;
    for (i=0; i<35; i++) {
        CONTEND_DELTA(crealf(buf_test[i]), crealf(buf_2[i]), tol);
        CONTEND_DELTA(cimagf(buf_test[i]), cimagf(buf_2[i]), tol);
    }
}

