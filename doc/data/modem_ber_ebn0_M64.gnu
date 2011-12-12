#
# modem_ber_ebn0_M64.gnu
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
    'data/ber-modem/ber_arb64opt.dat'    using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '64-QAM (opt)',\
    'data/ber-modem/ber_qam64.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#333333' title '64-QAM',\
    'data/ber-modem/ber_apsk64.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#666666' title '64-APSK',\
    'data/ber-modem/ber_arb64vt.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#999999' title '64-VT',\
    'data/ber-modem/ber_psk64.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#bbbbbb' title '64-PSK'
