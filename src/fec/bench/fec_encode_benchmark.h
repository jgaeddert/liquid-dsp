#ifndef __LIQUID_FEC_ENCODE_BENCHMARK_H__
#define __LIQUID_FEC_ENCODE_BENCHMARK_H__

#include <sys/resource.h>
#include <stdlib.h>

#include "../src/fec.h"

#define FEC_ENCODE_BENCH_API(FS,N, OPT) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fec_encode_bench(_start, _finish, _num_iterations, FS, N, OPT); }

// Helper function to keep code base small
void fec_encode_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    fec_scheme _fs,
    unsigned int _n,
    void * _opts)
{
    // generate fec object
    fec q = fec_create(_fs,_n,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // encoded message

    // initialze message
    unsigned long int i;
    for (i=0; i<_n; i++) {
        msg[i] = rand() & 0xff;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_encode(q,msg,msg_enc);
        fec_encode(q,msg,msg_enc);
        fec_encode(q,msg,msg_enc);
        fec_encode(q,msg,msg_enc);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    fec_destroy(q);
}

//
// BENCHMARKS
//
void benchmark_fec_enc_rep3_n4          FEC_ENCODE_BENCH_API(FEC_REP3, 4,   NULL)
void benchmark_fec_enc_rep3_n16         FEC_ENCODE_BENCH_API(FEC_REP3, 16,  NULL)
void benchmark_fec_enc_rep3_n64         FEC_ENCODE_BENCH_API(FEC_REP3, 64,  NULL)
void benchmark_fec_enc_rep3_n256        FEC_ENCODE_BENCH_API(FEC_REP3, 256, NULL)

void benchmark_fec_enc_hamming74_n4     FEC_ENCODE_BENCH_API(FEC_HAMMING74, 4,   NULL)
void benchmark_fec_enc_hamming74_n16    FEC_ENCODE_BENCH_API(FEC_HAMMING74, 16,  NULL)
void benchmark_fec_enc_hamming74_n64    FEC_ENCODE_BENCH_API(FEC_HAMMING74, 64,  NULL)
void benchmark_fec_enc_hamming74_n256   FEC_ENCODE_BENCH_API(FEC_HAMMING74, 256, NULL)

#endif // __LIQUID_FEC_ENCODE_BENCHMARK_H__

