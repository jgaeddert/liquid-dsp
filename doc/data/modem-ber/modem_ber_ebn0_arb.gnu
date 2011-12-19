#
# modem_ber_ebn0_arb.gnu
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
    'figures.gen/ber_ook.dat'        using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'OOK',\
    'figures.gen/ber_V29.dat'        using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'V29',\
    'figures.gen/ber_sqam32.dat'     using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'square 32-QAM',\
    'figures.gen/ber_sqam128.dat'    using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'square 128-QAM',\
    'figures.gen/ber_arb16opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'optimal 16-QAM',\
    'figures.gen/ber_arb32opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'optimal 32-QAM',\
    'figures.gen/ber_arb64vt.dat'    using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '64-QAM VT logo'
