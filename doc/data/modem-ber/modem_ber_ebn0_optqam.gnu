#
# modem_ber_ebn0_optqam.gnu
#
# Optimal QAM modems
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
set key top right nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'figures.gen/ber_qpsk.dat'       using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title 'QPSK',\
    'figures.gen/ber_apsk8.dat'      using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#222222' title '8-APSK',\
    'figures.gen/ber_arb16opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#444444' title '16-QAM (opt)',\
    'figures.gen/ber_arb32opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#666666' title '32-QAM (opt)',\
    'figures.gen/ber_arb64opt.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#888888' title '64-QAM (opt)',\
    'figures.gen/ber_arb128opt.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#aaaaaa' title '128-QAM (opt)',\
    'figures.gen/ber_arb256opt.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#cccccc' title '256-QAM (opt)'
