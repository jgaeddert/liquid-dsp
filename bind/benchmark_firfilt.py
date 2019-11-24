#!/usr/bin/env python3
import sys, time
sys.path.append('.')
import numpy as np
import liquid as dsp

def benchmark_firfilt(h_len,runtime=1.0):
    # design prototype filter
    filt = dsp.firfilt("lowpass", n=h_len, fc=0.25)

    # run in blocks until about time has elapsed
    block_size = int(500e3)
    buf = np.zeros((block_size,), dtype=np.csingle)
    tic = time.time()
    n   = 0
    while time.time() - tic < runtime:
        filt.execute(buf)
        n += block_size

    # print results
    t = time.time() - tic
    print("%6u: %12u samples in %6.3f seconds (%8.3f Ms/s)" % (h_len,n,t,n/t*1e-6))

# run batch of benchmarks
for h_len in (4,8,16,32,64,128,256,512,1024,):
    benchmark_firfilt(h_len)

