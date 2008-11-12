#ifndef __LIQUID_FEC_BENCHMARK_H__
#define __LIQUID_FEC_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/fec.h"

#define BSIZE   64

//
// BENCHMARK: Repeat/3 codec
//
void benchmark_rep3_encode(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;
    unsigned int n=BSIZE;
    unsigned char msg_dec[n], msg_enc[3*n];

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_rep3_encode(msg_dec, n, msg_enc);
        fec_rep3_encode(msg_dec, n, msg_enc);
        fec_rep3_encode(msg_dec, n, msg_enc);
        fec_rep3_encode(msg_dec, n, msg_enc);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;
}

void benchmark_rep3_decode(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;
    unsigned int n=BSIZE;
    unsigned char msg_dec[n], msg_enc[3*n];

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_rep3_decode(msg_enc, n, msg_dec);
        fec_rep3_decode(msg_enc, n, msg_dec);
        fec_rep3_decode(msg_enc, n, msg_dec);
        fec_rep3_decode(msg_enc, n, msg_dec);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;
}

//
// BENCHMARK: Hamming (7,4) codec
//
void benchmark_hamming74_encode(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;
    unsigned int n=BSIZE;
    unsigned char msg_dec[n], msg_enc[2*n];

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_hamming74_encode(msg_dec, n, msg_enc);
        fec_hamming74_encode(msg_dec, n, msg_enc);
        fec_hamming74_encode(msg_dec, n, msg_enc);
        fec_hamming74_encode(msg_dec, n, msg_enc);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;
}

void benchmark_hamming74_decode(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;
    unsigned int n=BSIZE;
    unsigned char msg_dec[n], msg_enc[2*n];

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_hamming74_decode(msg_enc, n, msg_dec);
        fec_hamming74_decode(msg_enc, n, msg_dec);
        fec_hamming74_decode(msg_enc, n, msg_dec);
        fec_hamming74_decode(msg_enc, n, msg_dec);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;
}

#endif // __LIQUID_FEC_BENCHMARK_H__

