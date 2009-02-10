#ifndef __LIQUID_MODEM_DEMODULATE_BENCHMARK_H__
#define __LIQUID_MODEM_DEMODULATE_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define MODEM_DEMODULATE_BENCH_API(MS,BPS) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ modem_demodulate_bench(_start, _finish, _num_iterations, MS, BPS); }

// Helper function to keep code base small
void modem_demodulate_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    modulation_scheme _ms,
    unsigned int _bps)
{
    // initialize modulator
    modem demod = modem_create(_ms, _bps);

    float complex x = 1.0f;
    unsigned int symbol_out;
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        demodulate(demod, x, &symbol_out);
        demodulate(demod, x, &symbol_out);
        demodulate(demod, x, &symbol_out);
        demodulate(demod, x, &symbol_out);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    free_modem(demod);
}

// specific modems
void benchmark_demodulate_bpsk      MODEM_DEMODULATE_BENCH_API(MOD_BPSK, 1)
void benchmark_demodulate_qpsk      MODEM_DEMODULATE_BENCH_API(MOD_QPSK, 2)

// ASK
void benchmark_demodulate_ask2      MODEM_DEMODULATE_BENCH_API(MOD_ASK, 1)
void benchmark_demodulate_ask4      MODEM_DEMODULATE_BENCH_API(MOD_ASK, 2)
void benchmark_demodulate_ask8      MODEM_DEMODULATE_BENCH_API(MOD_ASK, 3)
void benchmark_demodulate_ask16     MODEM_DEMODULATE_BENCH_API(MOD_ASK, 4)

// PSK
void benchmark_demodulate_psk2      MODEM_DEMODULATE_BENCH_API(MOD_PSK, 1)
void benchmark_demodulate_psk4      MODEM_DEMODULATE_BENCH_API(MOD_PSK, 2)
void benchmark_demodulate_psk8      MODEM_DEMODULATE_BENCH_API(MOD_PSK, 3)
void benchmark_demodulate_psk16     MODEM_DEMODULATE_BENCH_API(MOD_PSK, 4)
void benchmark_demodulate_psk32     MODEM_DEMODULATE_BENCH_API(MOD_PSK, 5)
void benchmark_demodulate_psk64     MODEM_DEMODULATE_BENCH_API(MOD_PSK, 6)

// Differential PSK
void benchmark_demodulate_dpsk2     MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 1)
void benchmark_demodulate_dpsk4     MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 2)
void benchmark_demodulate_dpsk8     MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 3)
void benchmark_demodulate_dpsk16    MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 4)
void benchmark_demodulate_dpsk32    MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 5)
void benchmark_demodulate_dpsk64    MODEM_DEMODULATE_BENCH_API(MOD_DPSK, 6)

// QAM
void benchmark_demodulate_qam4     MODEM_DEMODULATE_BENCH_API(MOD_QAM, 2)
void benchmark_demodulate_qam8     MODEM_DEMODULATE_BENCH_API(MOD_QAM, 3)
void benchmark_demodulate_qam16    MODEM_DEMODULATE_BENCH_API(MOD_QAM, 4)
void benchmark_demodulate_qam32    MODEM_DEMODULATE_BENCH_API(MOD_QAM, 5)
void benchmark_demodulate_qam64    MODEM_DEMODULATE_BENCH_API(MOD_QAM, 6)
void benchmark_demodulate_qam128   MODEM_DEMODULATE_BENCH_API(MOD_QAM, 7)
void benchmark_demodulate_qam256   MODEM_DEMODULATE_BENCH_API(MOD_QAM, 8)

#endif // __LIQUID_MODEM_DEMODULATE_BENCHMARK_H__

