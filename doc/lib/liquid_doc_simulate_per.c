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
    unsigned int bps                = _opts.bps;
    fec_scheme fec0                 = _opts.fec0;
    fec_scheme fec1                 = _opts.fec1;
    unsigned int dec_msg_len        = _opts.dec_msg_len;

    // stopping criteria
    unsigned long int min_packet_errors  = _opts.min_packet_errors;
    unsigned long int min_bit_errors     = _opts.min_bit_errors;
    unsigned long int min_packet_trials  = _opts.min_packet_trials;
    unsigned long int min_bit_trials     = _opts.min_bit_trials;
    unsigned long int max_packet_trials  = _opts.max_packet_trials;
    unsigned long int max_bit_trials     = _opts.max_bit_trials;

    // create objects
    crc_scheme crc = LIQUID_CRC_32;
    packetizer p = packetizer_create(dec_msg_len,crc,fec0,fec1);

    // create modem
    modem mod   = modem_create(ms, bps);
    modem demod = modem_create(ms, bps);

    // derived values
    unsigned int enc_msg_len = packetizer_get_enc_msg_len(p);
    // modulated data
    div_t d;
    // ensure msg_length is long enough
    d = div(8*enc_msg_len, bps);
    unsigned int num_symbols = d.quot + (d.rem ? 1 : 0);
    float nstd = powf(10.0f, -_SNRdB / 20.0f);
    //printf("   nstd : %12.8f\n", nstd);

    // bookkeeping variables
    unsigned long int i;
    unsigned long int num_bit_trials;
    unsigned long int num_bit_errors = 0;
    unsigned long int num_packet_trials;
    unsigned long int num_packet_errors = 0;
    unsigned int num_written;           // for byte packing
    int continue_trials = 1;    // inner loop flag
    int success = 1;

    // data arrays
    unsigned char msg_org[dec_msg_len];     // original message
    unsigned char msg_enc[enc_msg_len];     // encoded message
    unsigned char msg_mod[num_symbols];     // modulated message (symbols)
    unsigned char msg_dem[num_symbols];     // demodulatd message (symbols)
    unsigned char msg_rec[enc_msg_len+8];   // received message (with padding)
    unsigned char msg_dec[dec_msg_len];     // decoded message

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
        packetizer_encode(p, msg_org, msg_enc);

        // 3. format encoded data into symbols
        //memset(msg, 0, msg_len*sizeof(unsigned char));
        liquid_repack_bytes(msg_enc,    8,      enc_msg_len,
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
            modem_demodulate(demod, s, &demod_sym);
            msg_dem[i] = (unsigned char)demod_sym;

            //msg_dem[i] = msg_mod[i];
        }

        // 7. unpack demodulated symbols
        liquid_repack_bytes(msg_dem,   bps,    num_symbols,
                            msg_rec,   8,      enc_msg_len+8, // with padding
                            &num_written);
        
        // 8. decode
        //fec_decode(codec, framebytes, rx_enc_data, dec_data);
        int crc_pass =
        packetizer_decode(p, msg_rec, msg_dec);

        // 9. count errors
        unsigned int num_trial_bit_errors = 0;
        for (i=0; i<dec_msg_len; i++) {
            num_trial_bit_errors += count_bit_errors(msg_org[i], msg_dec[i]);
        }
        //printf("num errors: %u / %u\n", num_trial_bit_errors, 8*framebytes);

        num_bit_errors += num_trial_bit_errors;
        num_bit_trials += 8*dec_msg_len;

        num_packet_errors += crc_pass ? 0 : 1;
        num_packet_trials++;

        //
        // check stopping criteria
        //

        // have we reached out minimum errors and minimum trials criteria?
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
    packetizer_destroy(p);
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

#if 0
    printf(" %c SNR: %6.2f, bits: %8lu / %8lu (%12.4e), packets: %6lu / %6lu (%6.2f%%)\n",
            success ? '*' : ' ',
            _SNRdB,
            num_bit_errors,     num_bit_trials,     BER,
            num_packet_errors,  num_packet_trials,  PER*100.0f);
#endif
}

