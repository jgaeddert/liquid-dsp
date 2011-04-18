// file: doc/listings/gport.direct.example.c
#include <liquid/liquid.h>

int main() {
    // create the port
    //     size :   1024 
    //     type :   int
    gport p = gport_create(1024,sizeof(int));

    // producer requests 256 samples (blocking)
    int * w;
    w = (int*) gport_producer_lock(p,256);

    // producer writes data to w here

    // once data are written, producer unlocks the port
    gport_producer_unlock(p,256);

    // repeat as necessary

    // destroy the port object
    gport_destroy(p);
}
