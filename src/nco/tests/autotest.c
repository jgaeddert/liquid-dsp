// 
// Modem testsuite, main program
//

#include "../../../autotest/autotest.h"
#include "nco_autotest.h"

int main() {
    
    autotest_nco_basic();
    autotest_nco_mixing();

    autotest_print_results();
    printf("autotest complete.\n");
    return 0;
}

