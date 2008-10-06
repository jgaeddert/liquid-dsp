// 
// Modem testsuite, main program
//

#include "../../../autotest/autotest.h"
#include "modem_autotest.h"

int main() {
    
    autotest_mod_demod();

    autotest_print_results();
    printf("autotest complete.\n");
    return 0;
}

