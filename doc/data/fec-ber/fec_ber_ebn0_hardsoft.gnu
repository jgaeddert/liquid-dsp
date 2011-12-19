#
# fec_ber_ebn0_hardsoft.gnu
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
set key top right nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'data/fec-ber/hard/ber_h84.dat' using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#087848' title 'hard',\
    'data/fec-ber/soft/ber_h84.dat' using 2:3 with linespoints linewidth 1.2 pointtype 6 linecolor rgb '#e59710' title 'soft'
