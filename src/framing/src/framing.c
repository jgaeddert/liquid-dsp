//
//
//

#include "framing.h"

const struct framing_mod_bps framing_mod_bps_tab[32] = {
    // phase-shift keying
    {MOD_BPSK,      1},
    {MOD_QPSK,      2},
    {MOD_PSK,       3},
    {MOD_PSK,       4},
    {MOD_PSK,       5},
    {MOD_PSK,       6},

    // differential phase-shift keying
    {MOD_DPSK,      1},
    {MOD_DPSK,      2},
    {MOD_DPSK,      3},
    {MOD_DPSK,      4},
    {MOD_DPSK,      5},
    {MOD_DPSK,      6},

    // amplitude-shift keying
    {MOD_ASK,       2},
    {MOD_ASK,       3},
    {MOD_ASK,       4},
    {MOD_ASK,       5},
    {MOD_ASK,       6},

    // quadrature amplitude-shift keying
    {MOD_QAM,       2},
    {MOD_QAM,       3},
    {MOD_QAM,       4},
    {MOD_QAM,       5},
    {MOD_QAM,       6},
    {MOD_QAM,       7},
    {MOD_QAM,       8},

    // placeholders
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0},
    {MOD_UNKNOWN,   0}
};

