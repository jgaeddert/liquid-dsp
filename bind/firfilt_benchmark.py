#!/usr/bin/env python3
'''benchmark firfilt object'''
import argparse, sys, time
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

def benchmark_firfilt(h_len,runtime=1.0):
    # design prototype filter
    filt = dsp.firfilt("lowpass", n=h_len, fc=0.25)

    # run in blocks until about time has elapsed
    block_size = int(500e3)
    buf = np.zeros((block_size,), dtype=np.csingle)
    print('%4u:' % h_len, end='')
    for func,desc in ((filt.execute,'out of place',),(filt.execute_in_place,'in place')):
        tic = time.time()
        n   = 0
        while time.time() - tic < runtime:
            func(buf)
            n += block_size

        # print results
        t = time.time() - tic
        print(' %s: %6.2fM/%5.3fs (%7.3f Ms/s)' % (desc,n*1e-6, t, n/t*1e-6), end='')
    print('', end='\n')

# run batch of benchmarks
for h_len in (4,8,16,32,64,128,256,512,1024,):
    benchmark_firfilt(h_len)

