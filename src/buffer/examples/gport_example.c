//
// gport (generic port) example
//

#include <stdio.h>
#include <stdlib.h>

#include "../src/buffer.h"

int main() {
    // options
    unsigned int n=16;

    gport p = gport_create(n,sizeof(int));
    
    gport_print(p);

    gport_destroy(p);

    printf("done.\n");
    return 0;
}
