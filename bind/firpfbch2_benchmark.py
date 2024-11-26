#!/usr/bin/env python3
'''benchmark firpfbch2 object'''
import argparse, time
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

def benchmark_firpfbch2(M,runtime=1.0):
    # design prototype filter
    qa = dsp.firpfbch2a(M, m=2, As=60)
    qs = dsp.firpfbch2s(M, m=2, As=60)

    # run in blocks until about time has elapsed
    M2 = M//2
    b0 = np.zeros((M2,), dtype=np.csingle)
    b1 = np.zeros((M ,), dtype=np.csingle)
    print('%4u:' % M, end='')
    for q,desc,buf in ((qa,'analysis',b0),(qs,'synthesis',b1)):
        tic = time.time()
        n   = 0
        while time.time() - tic < runtime:
            q.execute(buf)
            n += M2

        # print results
        t = time.time() - tic
        print(' %s: %6.2fM/%5.3fs (%7.3f Ms/s)' % (desc,n*1e-6, t, n/t*1e-6), end='')
    print('', end='\n')

# run batch of benchmarks
for M in (16,32,64,128,256,512,1024,2048,4096,):
    benchmark_firpfbch2(M)

