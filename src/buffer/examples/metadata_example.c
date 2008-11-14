//
//
//

#include <stdio.h>

#include "../src/metadata.h"

int main() {

    metadata m = metadata_create();
    metadata_add_key(m, "mod-scheme", 1);
    metadata_add_key(m, "bps", 3);
    metadata_add_key(m, "tx-power", -12.3);
    metadata_add_key(m, "fec-scheme", 2);

    metadata_print(m);

    metadata_update(m, "bps", 4.0f);
    metadata_print(m);

    metadata_destroy(m);

    printf("done.\n");
    return 0;
}

