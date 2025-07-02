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

// multiply two complex buffers (simple test with visual result)
void autotest_vectorf_mul_4()
{
    float tol = 4e-6; // error tolerance
    float buf_0[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float buf_1[4] = {1.0f,-1.0f, 2.0f,-0.5f};

    // run vector multiplication and check result
    float buf_test[4];
    liquid_vectorf_mul(buf_0, buf_1, 4, buf_test);

    CONTEND_DELTA(buf_test[0],  1.0f, tol);
    CONTEND_DELTA(buf_test[1], -2.0f, tol);
    CONTEND_DELTA(buf_test[2],  6.0f, tol);
    CONTEND_DELTA(buf_test[3], -2.0f, tol);
}

// multiply two complex buffers
void autotest_vectorf_mul_16()
{
    // error tolerance
    float tol = 4e-6;

    float buf_0[16] = {
      1.07246623,  1.08146290, -1.14028088,  1.83380899,
     -0.05477144,  0.60832595,  1.81667523, -1.12238075,
      0.55524695, -1.94931519, -0.87191170,  0.91693119,
     -0.80607338,  0.76477512,  0.05099755, -0.87350051,
    };

    float buf_1[16] = {
      0.03785571,  0.72763665, -1.20262636, -0.88838102,
      1.58354529, -0.50696037,  1.40929619,  0.87868803,
      0.24852918, -0.62409860, -0.87039907,  0.90921212,
      0.33669564,  0.39064649, -2.45003867, -0.54862205,
    };

    // expected output
    float buf_2[16] = {
      0.04059897,  0.78691202,  1.37133181, -1.62912107,
     -0.08673306, -0.30839717,  2.56023335, -0.98622251,
      0.13799506,  1.21656489,  0.75891113,  0.83368498,
     -0.27140138,  0.29875672, -0.12494598,  0.47922167,
    };

    // run vector multiplication
    float buf_test[16];
    liquid_vectorf_mul(buf_0, buf_1, 16, buf_test);

    //compare result
    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA(buf_test[i], buf_2[i], tol);
}

//
void autotest_vectorf_mul_35()
{
    float tol = 4e-6;

    float buf_0[35] = {
      1.11555653, -0.36133676,  0.17714505,  2.20424609,
     -0.44699194,  0.60613931, -1.18746289, -0.46277775,
      0.93796307, -2.18699829,  0.16145611, -0.62653631,
     -0.67042462,  0.70333438, -0.32923580,  0.27534332,
      0.41492559,  3.25368958,  1.63002035,  2.22057186,
      1.40896777, -0.22334033, -1.48631186, -1.91632185,
     -0.52054895, -1.56515607, -0.91107117, -0.77007659,
     -0.46645585,  0.80317663,  0.26992512, -0.72456446,
      1.19573306,  1.42491943,  1.08243614, };

    float buf_1[35] = {
     -0.82466736, -1.46176052, -1.28388174, -0.51910014,
     -0.65964708, -1.40213479,  0.86051446,  0.26257342,
      0.72174183, -0.26018863,  0.57949117,  0.84635184,
     -1.12637629, -1.04214881,  0.54458433, -0.17847893,
      0.19642532, -1.84958229, -1.49042886, -0.27466940,
      0.29239375, -1.20243456, -0.41784260,  0.37737465,
     -1.25500377,  1.55976056, -0.05111343, -1.92966664,
     -2.82387897, -1.48171326,  0.38047273,  1.71995272,
      0.56040910, -0.46653022,  0.15515755, };

    float buf_2[35] = {
     -0.91996300,  0.52818781, -0.22743329, -1.14422441,
      0.29485694, -0.84988898, -1.02182901, -0.12151314,
      0.67696720,  0.56903213,  0.09356239, -0.53027016,
      0.75515038, -0.73297912, -0.17929666, -0.04914298,
      0.08150189, -6.01796675, -2.42942929, -0.60992312,
      0.41197336,  0.26855212,  0.62104446, -0.72317129,
      0.65329093, -2.44126868,  0.04656797,  1.48599112,
      1.31721485, -1.19007754,  0.10269915, -1.24621654,
      0.67009974, -0.66476804,  0.16794813, };

    // run vector multiplication
    float buf_test[35];
    liquid_vectorf_mul(buf_0, buf_1, 35, buf_test);

    //compare result
    unsigned int i;
    for (i=0; i<35; i++)
        CONTEND_DELTA(buf_test[i], buf_2[i], tol);
}

// multiply by scalar
void autotest_vectorf_mulscalar_4()
{
    float tol = 4e-6; // error tolerance
    float buf_0[4] = {1.0f, 2.0f,-3.0f, 4.0f};

    // run vector multiplication and check result
    float buf_test[4];
    liquid_vectorf_mulscalar(buf_0, 4, 2.0f, buf_test);

    CONTEND_DELTA(buf_test[0],  2.0f, tol);
    CONTEND_DELTA(buf_test[1],  4.0f, tol);
    CONTEND_DELTA(buf_test[2], -6.0f, tol);
    CONTEND_DELTA(buf_test[3],  8.0f, tol);
}

void autotest_vectorf_mulscalar_35()
{
    float tol = 4e-6;

    float buf_0[35] = {
      1.11555653, -0.36133676,  0.17714505,  2.20424609,
     -0.44699194,  0.60613931, -1.18746289, -0.46277775,
      0.93796307, -2.18699829,  0.16145611, -0.62653631,
     -0.67042462,  0.70333438, -0.32923580,  0.27534332,
      0.41492559,  3.25368958,  1.63002035,  2.22057186,
      1.40896777, -0.22334033, -1.48631186, -1.91632185,
     -0.52054895, -1.56515607, -0.91107117, -0.77007659,
     -0.46645585,  0.80317663,  0.26992512, -0.72456446,
      1.19573306,  1.42491943,  1.08243614, };

    float buf_2[35] = {
      0.78869843, -0.25546509,  0.12524155,  1.55840194,
     -0.31602332,  0.42854050, -0.83953631, -0.32718387,
      0.66313988, -1.54620790,  0.11414947, -0.44296119,
     -0.47399023,  0.49725744, -0.23276971,  0.19466774,
      0.29335240,  2.30035853,  1.15242445,  1.56994426,
      0.99614018, -0.15790161, -1.05082250, -1.35483956,
     -0.36802810, -1.10656536, -0.64412737, -0.54444414,
     -0.32978430,  0.56784588,  0.19083706, -0.51226705,
      0.84538329,  1.00741816,  0.76528233, };

    // run vector multiplication
    float buf_test[35];
    liquid_vectorf_mulscalar(buf_0, 35, 0.707f, buf_test);

    //compare result
    unsigned int i;
    for (i=0; i<35; i++)
        CONTEND_DELTA(buf_test[i], buf_2[i], tol);
}

