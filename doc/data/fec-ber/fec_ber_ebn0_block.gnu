#
# fec_ber_ebn0_block.gnu
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
    'data/fec-ber/hard/ber_h74.dat'         using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#008040' title 'Hamming(7,4)',\
    'data/fec-ber/hard/ber_secded2216.dat'  using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#8080c0' title 'SEC-DED(22,16)',\
    'data/fec-ber/hard/ber_h128.dat'        using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#004040' title 'Hamming(12,8)',\
    'data/fec-ber/hard/ber_secded3932.dat'  using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#5050c0' title 'SEC-DED(39,32)',\
    'data/fec-ber/hard/ber_secded7264.dat'  using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#2020c0' title 'SEC-DED(72,64)',\
    'data/fec-ber/hard/ber_g2412.dat'       using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#ad9120' title 'Golay(24,12)'

#    'data/fec-ber/hard/ber_h84.dat'      using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#008040' title 'Hamming(8,4)'
#    'data/fec-ber/hard/ber_rep3.dat'     using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#403000' title 'Repeat(3)',\
#    'data/fec-ber/hard/ber_rep5.dat'     using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#806000' title 'Repeat(5)',\
