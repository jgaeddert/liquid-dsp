// file: doc/listings/ofdmflexframesync.example.c
#include <liquid/liquid.h>

// callback function
int mycallback(unsigned char *  _header,
               int              _header_valid,
               unsigned char *  _payload,
               unsigned int     _payload_len,
               int              _payload_valid,
               framesyncstats_s _stats,
               void *           _userdata)
{
    printf("***** callback invoked!\n");
    return 0;
}

int main() {
    // options
    unsigned int M = 64;        // number of subcarriers
    unsigned int cp_len = 16;   // cyclic prefix length
    unsigned char p[M];         // subcarrier allocation (null/pilot/data)
    void * userdata;            // user-defined data
    
    // initialize subcarrier allocation to default
    ofdmframe_init_default_sctype(M, p);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, p, mycallback, userdata);

    // grab samples from source and push through synchronizer
    float complex buffer[20];   // time-domain buffer (any length)
    {
        // push received samples through synchronizer
        ofdmflexframesync_execute(fs, buffer, 20);
    }

    // destroy the frame synchronizer object
    ofdmflexframesync_destroy(fs);
}
