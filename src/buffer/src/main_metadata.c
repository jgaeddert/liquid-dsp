//
//
//

#include <stdio.h>

#include "metadata.h"

int main() {

    //metadata m = metadata_create(4, "mod-scheme", 1, "bps", 3);
    metadata m = metadata_create(2, "mod-scheme", 1.0, "bps", 3.0);

    metadata_print(m);

    metadata_update(m, "bps", 4.0f);
    metadata_print(m);

    metadata_destroy(m);

    printf("done.\n");
    return 0;
}

