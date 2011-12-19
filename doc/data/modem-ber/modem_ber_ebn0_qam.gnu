#
# modem_ber_ebn0_qam.gnu
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
    'figures.gen/ber_qam4.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '4-QAM',\
    'figures.gen/ber_qam8.dat'   using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '8-QAM',\
    'figures.gen/ber_qam16.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '16-QAM',\
    'figures.gen/ber_qam32.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '32-QAM',\
    'figures.gen/ber_qam64.dat'  using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '64-QAM',\
    'figures.gen/ber_qam128.dat' using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '128-QAM',\
    'figures.gen/ber_qam256.dat' using 2:3 with linespoints linewidth 2.0 pointtype 6 linecolor rgb '#000000' title '256-QAM'
