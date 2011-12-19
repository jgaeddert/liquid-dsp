#
# fec_ber_ebn0_convpunc.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [-8:10]
set yrange [1e-5:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.65
set size 1.0
set xlabel 'E_b/N_0 [dB]'
set ylabel 'BER'
set grid xtics ytics
set pointsize 0.6
set key bottom left nobox

set pointsize 0.5

# theoretical uncoded BPSK error curve
ber_bpsk(x) = 0.5*erfc(10**(x/20))

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    ber_bpsk(x) with lines linetype 1 linewidth 2.0 linecolor rgb '#000000' title 'Uncoded',\
    'data/fec-ber/hard/ber_v27.dat'      using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#871288' title 'conv. r1/2, K=7',\
    'data/fec-ber/hard/ber_v27p23.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#774298' title 'conv. r2/3, K=7',\
    'data/fec-ber/hard/ber_v27p34.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#6752a8' title 'conv. r3/4, K=7',\
    'data/fec-ber/hard/ber_v27p45.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#5762b8' title 'conv. r4/5, K=7',\
    'data/fec-ber/hard/ber_v27p56.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#4772c8' title 'conv. r5/6, K=7',\
    'data/fec-ber/hard/ber_v27p67.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#3782d8' title 'conv. r6/7, K=7',\
    'data/fec-ber/hard/ber_v27p78.dat'   using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#2792e8' title 'conv. r7/8, K=7'
