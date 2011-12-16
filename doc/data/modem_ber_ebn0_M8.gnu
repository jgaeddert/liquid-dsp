#
# modem_ber_ebn0_M8.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [0:30]
set yrange [1e-5:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.6
set size 0.8
set xlabel 'E_b/N_0 [dB]'
set ylabel 'BER'
set grid xtics ytics
set pointsize 0.6
set key top right nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'figures.gen/ber_apsk8.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '8-APSK',\
    'figures.gen/ber_qam8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#333333' title '8-QAM',\
    'figures.gen/ber_psk8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#666666' title '8-PSK',\
    'figures.gen/ber_dpsk8.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#999999' title '8-DPSK',\
    'figures.gen/ber_ask8.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#cccccc' title '8-ASK'
