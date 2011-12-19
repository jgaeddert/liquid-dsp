#
# modem_phase_error_M128.gnu
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
    'data/modem-phase-error/qam128.dat'     using 1:3 with lines linewidth 2.0 linecolor rgb '#6677zz' title '128-QAM',\
    'data/modem-phase-error/arb128opt.dat'  using 1:3 with lines linewidth 2.0 linecolor rgb '#ccc55c' title '128-QAM (opt)',\
    'data/modem-phase-error/sqam128.dat'    using 1:3 with lines linewidth 2.0 linecolor rgb '#773333' title '128-SQAM',\
    'data/modem-phase-error/apsk128.dat'    using 1:3 with lines linewidth 2.0 linecolor rgb '#aa5533' title '128-APSK'
