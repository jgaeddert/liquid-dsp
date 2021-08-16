// frame gen/sync 64 C++ example
#include <stdio.h>
#include "fg64.hh"
#include "fs64.hh"

namespace dsp = liquid;

// context for passing to callback
struct context_s { int num_detected; };

// user-defined callback
int my_callback(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata)
{
    printf("callback invoked! payload: %s\n", _payload_valid ? "pass" : "FAIL");
    ((context_s *)_userdata)->num_detected++;
    return 0;
}

int main()
{
    // generate header and payload bytes
    uint8_t header[8], payload[64];
    for (auto i=0; i< 8; i++) header[i]  = rand() & 0xff;
    for (auto i=0; i<64; i++) payload[i] = rand() & 0xff;

    // create frame generator and write frame to buffer
    dsp::fg64 fg;
    unsigned int frame_len = fg.get_frame_length();
    std::complex<float> frame_buf[frame_len];
    fg.execute(header, payload, frame_buf);

    // create frame synchronizer and receive frame
    context_s context = {.num_detected=0};
    dsp::fs64 fs(my_callback, &context);
    fs.execute(frame_buf, frame_len);
    printf("number of packets detected: %u\n", context.num_detected);
    return 0;
}
