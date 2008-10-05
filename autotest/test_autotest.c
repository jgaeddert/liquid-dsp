//
//
//

#include <stdio.h>
#include "autotest.h"

int main() {

    CONTEND_EQUALITY(1,1);
    CONTEND_EQUALITY(1,2);

    CONTEND_INEQUALITY(1,1);
    CONTEND_INEQUALITY(1,2);

    CONTEND_GREATER_THAN(1,1);
    CONTEND_GREATER_THAN(1,2);

    CONTEND_LESS_THAN(1,1);
    CONTEND_LESS_THAN(1,2);

    CONTEND_DELTA(1.0f, 1.0f, 0.1f);
    CONTEND_DELTA(1.0f, 2.0f, 0.1f);


    int x = 3, y = 5;

    CONTEND_INEQUALITY(x,y);

    autotest_print_results();

    printf("done.\n");
    return 0;
}

