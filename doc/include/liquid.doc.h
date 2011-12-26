/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

#ifndef __LIQUID_DOC_H__
#define __LIQUID_DOC_H__

#include <stdio.h>
#include <complex.h>
#include "liquid.h"

// default gnuplot colors
#define LIQUID_DOC_COLOR_GRID   "#cccccc"   // very light gray
#define LIQUID_DOC_COLOR_GRAY   "#999999"   // light gray
#define LIQUID_DOC_COLOR_BLUE   "#004080"   // blue
#define LIQUID_DOC_COLOR_GREEN  "#008040"   // green
#define LIQUID_DOC_COLOR_RED    "#800000"   // dark red
#define LIQUID_DOC_COLOR_PURPLE "#400040"   // dark puple

// power spectral density window
typedef enum {
    LIQUID_DOC_PSDWINDOW_NONE=0,
    LIQUID_DOC_PSDWINDOW_HANN,
    LIQUID_DOC_PSDWINDOW_HAMMING
} liquid_doc_psdwindow;

// compute power spectral density (complex float input)
void liquid_doc_compute_psdcf(float complex * _x,
                              unsigned int _n,
                              float complex * _X,
                              unsigned int _nfft,
                              liquid_doc_psdwindow _wtype,
                              int _normalize);

// compute power spectral density (float input)
void liquid_doc_compute_psdf(float * _x,
                             unsigned int _n,
                             float complex * _X,
                             unsigned int _nfft,
                             liquid_doc_psdwindow _wtype,
                             int _normalize);


// Compute spectral response
void liquid_doc_freqz(float * _b,
                      unsigned int _nb,
                      float * _a,
                      unsigned int _na,
                      unsigned int _nfft,
                      float complex * _H);

// simulate packet error rate options
typedef struct {
    modulation_scheme ms;           // modulation scheme
    fec_scheme fec0;                // FEC codec (inner)
    fec_scheme fec1;                // FEC codec (outer)
    int interleaving;               // apply interleaving?
    unsigned int dec_msg_len;       // (decoded) message length (bytes)
    int soft_decoding;              // soft bit decoding?
    
    int verbose;                    // verbose output?

    unsigned long int min_packet_errors;    // minimum number of packet errors
    unsigned long int min_bit_errors;       // minimum number of bit errors
    unsigned long int min_packet_trials;    // minimum number of packet trials
    unsigned long int min_bit_trials;       // minimum number of bit trials
    unsigned long int max_packet_trials;    // maximum number of packet trials
    unsigned long int max_bit_trials;       // maximum number of bit trials
} simulate_per_opts;

// simulate packet error rate results
typedef struct {
    int success;                            // success?

    unsigned long int num_bit_errors;       // number of bit errors
    unsigned long int num_bit_trials;       // number of bit trials

    unsigned long int num_packet_errors;    // number of packet errors
    unsigned long int num_packet_trials;    // number of packet trials

    // derived values
    float BER;  // apparent BER
    float PER;  // apparent PER
} simulate_per_results;


// simulate packet error rate
//  _opts       :   simulation options
//  _SNRdB      :   signal-to-noise ratio [dB]
//  _results    :   simulation results
void simulate_per(simulate_per_opts _opts,
                  float _SNRdB,
                  simulate_per_results * _results);

#define ESTIMATE_SNR_BER    0
#define ESTIMATE_SNR_PER    1

#define ESTIMATE_SNR        0
#define ESTIMATE_EBN0       1
// find approximate SNR to achieve a particular BER/PER
//  _opts       :   options
//  _ber_per    :   use BER or PER?
//  _snr_ebn0   :   search for SNR or Eb/N0?
//  _error_rate :   target error rate
float estimate_snr(simulate_per_opts _opts,
                   int _ber_per,
                   int _snr_ebn0,
                   float _error_rate);

// solve for SNR (Eb/N0) for BPSK for a given BER
float estimate_snr_bpsk(float _error_rate);

#endif // __LIQUID_DOC_H__

