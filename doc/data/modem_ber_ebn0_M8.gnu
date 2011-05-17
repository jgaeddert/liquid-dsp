#
# modem_ber_ebn0_M8.gnu
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
    'data/ber-modem/ber_apsk8.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '8-APSK',\
    'data/ber-modem/ber_qam8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#444444' title '8-QAM',\
    'data/ber-modem/ber_psk8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#888888' title '8-PSK',\
    'data/ber-modem/ber_ask8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#cccccc' title '8-ASK'
