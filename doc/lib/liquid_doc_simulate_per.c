//
// liquid_doc_simulate_per.c
//

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

// simulate packet error rate
//  _opts       :   simulation options
//  _SNRdB      :   signal-to-noise ratio [dB]
//  _results    :   simulation results
void simulate_per(simulate_per_opts _opts,
                  float _SNRdB,
                  simulate_per_results * _results)
{
    // validate input
    if (_opts.dec_msg_len == 0) {
        fprintf(stderr,"error: _dec_msg_len must be greater than 0\n");
        exit(1);
    }

    modulation_scheme ms            = _opts.ms;
    fec_scheme fec0                 = _opts.fec0;
    fec_scheme fec1                 = _opts.fec1;
    unsigned int dec_msg_len        = _opts.dec_msg_len;
    int soft_decoding               = _opts.soft_decoding;

    if (fec1 != LIQUID_FEC_NONE) {
        fprintf(stderr,"warning: simulate_per(), forcing fec1 to be 'none'\n");
        fec1 = LIQUID_FEC_NONE;
    }

    // stopping criteria
    unsigned long int min_packet_errors  = _opts.min_packet_errors;
    unsigned long int min_bit_errors     = _opts.min_bit_errors;
    unsigned long int min_packet_trials  = _opts.min_packet_trials;
    unsigned long int min_bit_trials     = _opts.min_bit_trials;
    unsigned long int max_packet_trials  = _opts.max_packet_trials;
    unsigned long int max_bit_trials     = _opts.max_bit_trials;

    // create objects
    //crc_scheme crc = LIQUID_CRC_32;
    fec p = fec_create(fec0, NULL);

    // create interleaver
    interleaver intlv = interleaver_create(fec_get_enc_msg_length(fec0,dec_msg_len));
    if (_opts.interleaving) interleaver_set_depth(intlv,4);
    else                    interleaver_set_depth(intlv,0);

    // create modem
    modem mod   = modem_create(ms);
    modem demod = modem_create(ms);
    unsigned int bps = modem_get_bps(mod);

    // derived values
    unsigned int enc_msg_len = fec_get_enc_msg_length(fec0,dec_msg_len);
    // modulated data
    div_t d;
    // ensure msg_length is long enough
    d = div(8*enc_msg_len, bps);
    unsigned int num_symbols = d.quot + (d.rem ? 1 : 0);
    float nstd = powf(10.0f, -_SNRdB / 20.0f);
    //printf("   nstd : %12.8f\n", nstd);
    float rate = bps * fec_get_rate(_opts.fec0) * fec_get_rate(_opts.fec1);

    // bookkeeping variables
    unsigned long int i;
    unsigned long int num_bit_trials;
    unsigned long int num_bit_errors = 0;
    unsigned long int num_packet_trials;
    unsigned long int num_packet_errors = 0;
    unsigned int num_written;           // for byte packing
    int continue_trials = 1;    // inner loop flag
    int success = 0;

    // data arrays
    unsigned char msg_org[dec_msg_len];         // original message
    unsigned char msg_enc[enc_msg_len];         // encoded message
    unsigned char msg_int[enc_msg_len];         // interleaved message
    unsigned char msg_mod[num_symbols];         // modulated message (symbols)
    unsigned char msg_dem[num_symbols];         // demodulated message (symbols)
    unsigned char msg_dem_soft[8*num_symbols];  // demodulated message (soft bits)
    unsigned char msg_din_soft[8*num_symbols];  // de-interleaved message (soft bits)
    unsigned char msg_din[enc_msg_len+8];       // de-interleaved message (with padding)
    unsigned char msg_rec[enc_msg_len+8];       // received message (with padding)
    unsigned char msg_dec[dec_msg_len];         // decoded message

    //
    // ---------- BEGIN TRIALS ----------
    //

    // start trials for a particular SNR
    num_bit_trials = 0;
    num_bit_errors = 0;
    num_packet_trials = 0;
    num_packet_errors = 0;
    continue_trials = 1;
    do {

        // 1. init random data
        for (i=0; i<dec_msg_len; i++)
            msg_org[i] = rand() & 0xff;

        // 2. encode data
        fec_encode(p, dec_msg_len, msg_org, msg_enc);

        // interleave
        interleaver_encode(intlv, msg_enc, msg_int);

        // 3. format encoded data into symbols
        //memset(msg, 0, msg_len*sizeof(unsigned char));
        liquid_repack_bytes(msg_int,    8,     enc_msg_len,
                            msg_mod,    bps,   num_symbols,
                            &num_written);

        // 4. modulate
        float complex s;
        for (i=0; i<num_symbols; i++) {
            modem_modulate(mod, msg_mod[i], &s);

            // add noise
            cawgn(&s, nstd);
            //s += (randnf() + randnf()*_Complex_I)*nstd/sqrtf(2.0f);

            // demodulate
            unsigned int demod_sym;
            if (soft_decoding) {
                modem_demodulate_soft(demod, s, &demod_sym, &msg_dem_soft[bps*i]);
            } else {
                modem_demodulate(demod, s, &demod_sym);
            }
            msg_dem[i] = (unsigned char)demod_sym;

            //msg_dem[i] = msg_mod[i];
        }

        // 7. unpack demodulated symbols (hard decoding)
        if (!soft_decoding) {
            liquid_repack_bytes(msg_dem,   bps,    num_symbols,
                                msg_din,   8,      enc_msg_len+8, // with padding
                                &num_written);
        }

        // de-interleave
        if (soft_decoding) {
            interleaver_decode_soft(intlv, msg_dem_soft, msg_din_soft);
        } else {
            interleaver_decode(intlv, msg_din, msg_rec);
        }
        
        // 8. decode
        if (soft_decoding) {
            //fec_decode(codec, framebytes, rx_enc_data, dec_data);
            fec_decode_soft(p, dec_msg_len, msg_din_soft, msg_dec);
        } else {
            //fec_decode(codec, framebytes, rx_enc_data, dec_data);
            fec_decode(p, dec_msg_len, msg_rec, msg_dec);
        }

        // 9. count errors
        unsigned int num_trial_bit_errors = 0;
        num_trial_bit_errors = count_bit_errors_array(msg_org, msg_dec, dec_msg_len);
        //printf("num errors: %u / %u\n", num_trial_bit_errors, 8*framebytes);
        int crc_pass = (num_trial_bit_errors > 0) ? 0 : 1;

        num_bit_errors += num_trial_bit_errors;
        num_bit_trials += 8*dec_msg_len;

        num_packet_errors += crc_pass ? 0 : 1;
        num_packet_trials++;

        // peridically print results
        if ( (num_packet_trials % 50)==0 && _opts.verbose) {
            float BER = (float)num_bit_errors/(float)num_bit_trials;
            float PER = (float)num_packet_errors/(float)num_packet_trials;
            float rate = bps * fec_get_rate(_opts.fec0) * fec_get_rate(_opts.fec1);

            printf(" %c SNR: %6.2f, EbN0: %6.2f, bits: %7lu/%9lu (%8.4e), packets: %6lu/%6lu (%6.2f%%)\r",
                    success ? '*' : ' ',
                    _SNRdB,
                    _SNRdB - 10.0f*log10f(rate),
                    num_bit_errors,     num_bit_trials,     BER,
                    num_packet_errors,  num_packet_trials,  PER*100.0f);
            fflush(stdout);
        }

        //
        // check stopping criteria
        //

        // have we reached the minimum errors and minimum trials criteria?
        if (num_packet_errors >= min_packet_errors  && 
            num_bit_errors    >= min_bit_errors     &&
            num_packet_trials >= min_packet_trials  &&
            num_bit_trials    >= min_bit_trials)
        {
            // good enough
            continue_trials = 0;
            success = 1;
        }

        // have we exceeded the maximum number of trials?
        if (num_packet_trials >= max_packet_trials ||
            num_bit_trials    >= max_bit_trials)
        {
            // too many trials
            continue_trials = 0;
            success = 0;
        }

    } while (continue_trials);

    // clean up objects
    fec_destroy(p);
    modem_destroy(mod);
    modem_destroy(demod);

    float BER = (float)num_bit_errors/(float)num_bit_trials;
    float PER = (float)num_packet_errors/(float)num_packet_trials;

    // save results
    _results->success = success;
    _results->num_bit_errors = num_bit_errors;
    _results->num_bit_trials = num_bit_trials;
    _results->num_packet_errors = num_packet_errors;
    _results->num_packet_trials = num_packet_trials;
    _results->BER = BER;
    _results->PER = PER;

    if ( _opts.verbose) {
        printf(" %c SNR: %6.2f, EbN0: %6.2f, bits: %7lu/%9lu (%8.4e), packets: %6lu/%6lu (%6.2f%%)\n",
                success ? '*' : ' ',
                _SNRdB,
                _SNRdB - 10.0f*log10f(rate),
                num_bit_errors,     num_bit_trials,     BER,
                num_packet_errors,  num_packet_trials,  PER*100.0f);
    }
}

