// 
// Modem testsuite, main program
//

#include "../../../autotest/autotest.h"
#include "random_autotest.h"

int main() {
    
    autotest_randf();
    autotest_randnf();
    autotest_rand_weibullf();
    autotest_rand_ricekf();

    autotest_print_results();
    printf("autotest complete.\n");
    return 0;
}

