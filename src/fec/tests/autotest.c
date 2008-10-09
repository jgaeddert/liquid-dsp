// 
// Modem testsuite, main program
//

#include "../../../autotest/autotest.h"
#include "checksum_autotest.h"

int main() {
    
    autotest_basic_checksum();

    autotest_print_results();
    printf("autotest complete.\n");
    return 0;
}

