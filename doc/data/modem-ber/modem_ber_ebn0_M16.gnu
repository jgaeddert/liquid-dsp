#
# modem_ber_ebn0_M16.gnu
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
    'figures.gen/ber_arb16opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '16-QAM (opt)',\
    'figures.gen/ber_qam16.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#222222' title '16-QAM',\
    'figures.gen/ber_apsk16.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#444444' title '16-APSK',\
    'figures.gen/ber_V29.dat'        using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#666666' title 'V.29',\
    'figures.gen/ber_psk16.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#888888' title '16-PSK',\
    'figures.gen/ber_dpsk16.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#aaaaaa' title '16-DPSK',\
    'figures.gen/ber_ask16.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#cccccc' title '16-ASK'
