/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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

// compute emperical distributions and compare to theoretical

// add value to histogram
unsigned int _support_histogram_add(float        _value,
                                    float *      _bins,
                                    unsigned int _num_bins,
                                    float        _vmin,
                                    float        _vmax)
{
    float indexf = _num_bins * (_value - _vmin) / (_vmax - _vmin);
    unsigned int index = 0;
    if (indexf >= 0)
        index = (unsigned int)indexf;
    if (index >= _num_bins)
        index = _num_bins - 1;
    _bins[index]++;
    return index;
}

// noramlize histogram (area under curve)
float _support_histogram_normalize(float *      _bins,
                                   unsigned int _num_bins,
                                   float        _vmin,
                                   float        _vmax)
{
    float area = 0.0f;
    unsigned int i;
    for (i=0; i<_num_bins; i++)
        area += _bins[i];
    area *= (_vmax - _vmin) / (float)_num_bins;
    for (i=0; i<_num_bins; i++)
        _bins[i] /= area;
    return area;
}

// log histogram
void _support_histogram_log(float *      _bins,
                            unsigned int _num_bins,
                            float        _vmin,
                            float        _vmax)
{
    // find max(hist)
    unsigned int i;
    float hist_max = 0;
    for (i=0; i<_num_bins; i++)
        hist_max = _bins[i] > hist_max ? _bins[i] : hist_max;

    unsigned int num_chars = 72;
    float _vstep = (_vmax - _vmin) / (float)(_num_bins-1);
    printf("%8s : [%12s]\n", "v", "probability");
    for (i=0; i<_num_bins; i++) {
        printf("%8.2f : [%12g]", _vmin + i*_vstep, _bins[i]);

        unsigned int k;
        unsigned int n = round(num_chars * _bins[i] / hist_max);
        for (k=0; k<n; k++)
            printf("#");
        printf("\n");
    }
}

// normal distribution
void autotest_distribution_randnf()
{
    unsigned long int num_trials = 96000;
    unsigned long int i;
    float v;

    unsigned int num_bins = 31;
    float bins[num_bins];
    for (i=0; i<num_bins; i++)
        bins[i] = 0.0f;
    float vmin = -5.0f, vmax = +5.0f;

    // compute distribution
    for (i=0; i<num_trials; i++) {
        v = randnf();
        _support_histogram_add(v, bins, num_bins, vmin, vmax);
    }

    // noramlize distribution
    float area;
    area = _support_histogram_normalize(bins, num_bins, vmin, vmax);
    printf("area: %g\n", area);

    // log histogram
    _support_histogram_log(bins, num_bins, vmin, vmax);
}

