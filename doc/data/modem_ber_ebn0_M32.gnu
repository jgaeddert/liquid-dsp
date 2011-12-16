#
# modem_ber_ebn0_M32.gnu
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
    'figures.gen/ber_arb32opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '32-QAM (opt)',\
    'figures.gen/ber_sqam32.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#222222' title '32-SQAM',\
    'figures.gen/ber_apsk32.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#444444' title '32-APSK',\
    'figures.gen/ber_qam32.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#666666' title '32-QAM',\
    'figures.gen/ber_psk32.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#888888' title '32-PSK',\
    'figures.gen/ber_dpsk32.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#aaaaaa' title '32-DPSK'
