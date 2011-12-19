#
# fec_ber_ebn0_conv.gnu
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
    'data/fec-ber/hard/ber_v29.dat'      using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#0d90b5' title 'conv. r1/2, K=9',\
    'data/fec-ber/hard/ber_v39.dat'      using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#ff007e' title 'conv. r1/3, K=9',\
    'data/fec-ber/hard/ber_v615.dat'     using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#e59710' title 'conv. r1/6, K=15'
