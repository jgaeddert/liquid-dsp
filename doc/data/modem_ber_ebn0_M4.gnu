#
# modem_ber_ebn0_M4.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [0:30]
set yrange [1e-5:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.65
set size 1.0
set xlabel 'E_b/N_0 [dB]'
set ylabel 'BER'
set key bottom right nobox
set grid xtics ytics
set pointsize 0.6
set key bottom left nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'data/ber-modem/ber_qpsk.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'QPSK/4-QAM',\
    'data/ber-modem/ber_apsk4.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#444444' title '4-APSK',\
    'data/ber-modem/ber_dpsk4.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#888888' title '4-DPSK',\
    'data/ber-modem/ber_ask4.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#bbbbbb' title '4-ASK'
