#!/bin/bash
./bind/fdelay_example.py -nodisplay && \
./bind/firfilt_benchmark.py -nodisplay && \
./bind/firfilt_example.py -nodisplay && \
./bind/firinterp_example.py -nodisplay && \
./bind/firpfbch2_benchmark.py -nodisplay && \
./bind/firpfbch2_example.py -nodisplay && \
./bind/frame64_detect_example.py -nodisplay && \
./bind/frame64_example.py -nodisplay && \
./bind/msresamp_example.py -nodisplay && \
./bind/modem_example.py -nodisplay && \
./bind/ofdmflexframe_detect_example.py -nodisplay && \
./bind/ofdmflexframe_example.py -nodisplay && \
./bind/rresamp_example.py -nodisplay && \
./bind/spgram_example.py -nodisplay && \
./bind/spwaterfall_example.py -nodisplay && \
./bind/symstreamr_example.py -nodisplay && \
./bind/symtrack_example.py -nodisplay && \
echo " python tests pass" || exit 1

