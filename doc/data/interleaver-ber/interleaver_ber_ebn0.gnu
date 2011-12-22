#
# interleaver_ber_ebn0.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [-8:20]
set yrange [1e-5:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.65
set size 1.0
set xlabel 'E_b/N_0 [dB]'
set ylabel 'BER'
set grid xtics ytics
set pointsize 0.6
set key top right nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'data/interleaver-ber/interleaver_ber_ebn0_i4.dat' using 2:3 with linespoints linewidth 2 pointtype 6 linecolor rgb '#008040' title 'interleaving',\
    'data/interleaver-ber/interleaver_ber_ebn0_i0.dat' using 2:3 with linespoints linewidth 2 pointtype 6 linecolor rgb '#000000' title 'no interleaving'
