#!/usr/bin/env python3
'''Generate FFT benchmarks'''
import numpy as np

def isradix2(n):
    return 2**int(np.log2(n)) == n

def isprime(n):
    '''slow method but ok for small numbers'''
    if n < 2:
        return False
    if n in (2,3,5,7,):
        return True # for the sake of simplicity
    if (n & 1) == 0:
        return False
    for k in range(3,int(np.sqrt(n))+2,2):
        if n % k == 0:
            return False
    return True

nfft = tuple(range(2,512)) + tuple(2**n for n in range(2,16))

for n in sorted(tuple(set(nfft))):
    if   isradix2(n): ntype = 'radix2'
    elif isprime(n):  ntype = 'prime'
    else:             ntype = 'composite'

    print('LIQUID_BENCHMARK(fft_%-5d,"fft execute, nfft=%5u","fft,%s"%s)' % \
        (n,n,ntype,' '*(len('composite')-len(ntype))), end='')
    print('  { return fft_runbench(num_iterations, %5d, LIQUID_FFT_FORWARD); }' % (n))

