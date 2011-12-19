#
# modem_phase_error_qam.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [0:0.78540]
set size ratio 0.65
set size 1.0
set xlabel 'phase error'
set ylabel 'phase error (estimate)'
set key bottom right nobox
set grid xtics ytics
set pointsize 0.6
set key top left nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'data/modem-phase-error/qam4.dat'   using 1:($3*1) with lines linewidth 2.0 linecolor rgb '#000000' title '4-QAM',\
    'data/modem-phase-error/qam16.dat'  using 1:($3*1) with lines linewidth 2.0 linecolor rgb '#444444' title '16-QAM',\
    'data/modem-phase-error/qam64.dat'  using 1:($3*1) with lines linewidth 2.0 linecolor rgb '#888888' title '64-QAM',\
    'data/modem-phase-error/qam256.dat' using 1:($3*1) with lines linewidth 2.0 linecolor rgb '#cccccc' title '256-QAM'

#    'data/modem-phase-error/qam8.dat'   using 1:($3*3) with lines linewidth 2.0 linecolor rgb '#000000' title '8-QAM'
#    'data/modem-phase-error/qam32.dat'  using 1:($3*5) with lines linewidth 2.0 linecolor rgb '#000000' title '32-QAM'
#    'data/modem-phase-error/qam128.dat' using 1:($3*7) with lines linewidth 2.0 linecolor rgb '#000000' title '128-QAM'
