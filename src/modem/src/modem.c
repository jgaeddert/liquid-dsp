//
//
//

#include <stdlib.h>
#include "modem_internal.h"

// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod)
{
    return rand() % (_mod->M);
}

unsigned int modem_get_bps(modem _mod)
{
    return _mod->m;
}

