// frame gen/sync 64 C++ example
#include <stdio.h>
#include "fg64.hh"
#include "fs64.hh"

namespace dsp = liquid;

// TODO: make this easier for C++
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
    printf(" header: ");
    for (auto i=0; i<8; i++)
        printf(" %.2x", _header[i]);
    printf("\n payload:");
    for (auto i=0; i<64; i++)
        printf(" %.2x%s", _payload[i], (i+4)%11==0 || i==63 ? "\n" : "");
    return 0;
}

int main()
{
    // create frame generator
    dsp::fg64 fg;

    // create frame synchronizer
    dsp::fs64 fs(my_callback, NULL);

    // generate a frame
    uint8_t header[8], payload[64];
    for (auto i=0; i< 8; i++) header[i]  = rand() & 0xff;
    for (auto i=0; i<64; i++) payload[i] = rand() & 0xff;
    // frame buffer
    unsigned int frame_len = fg.get_frame_length();
    std::complex<float> frame_buf[frame_len];

    fg.execute(header, payload, frame_buf);
    fs.execute(frame_buf, frame_len);

    return 0;
}
