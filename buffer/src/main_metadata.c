//
//
//

#include <stdio.h>

#include "metadata.h"

int main() {

    //metadata m = metadata_create(4, "mod-scheme", 1, "bps", 3);
    metadata m = metadata_create(4, "mod-scheme", 1, "bps", 3);

    metadata_print(m);

    metadata_update(m, "hello", 55.0f);
    metadata_print(m);

    metadata_destroy(m);

    printf("done.\n");
    return 0;
}

