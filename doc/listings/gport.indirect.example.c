// file: doc/listings/gport.indirect.example.c
#include <liquid/liquid.h>

int main() {
    // create the port
    //     size :   1024
    //     type :   int
    gport p = gport_create(1024,sizeof(int));

    // create buffer for writing
    int w[256];

    // producer writes data to w here

    // producer writes 256 values to port
    gport_produce(p,(void*)w,256);

    // repeat as necessary

    // destroy the port object
    gport_destroy(p);
}
