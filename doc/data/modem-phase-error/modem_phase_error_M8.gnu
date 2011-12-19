#
# modem_phase_error_M8.gnu
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
    'data/modem-phase-error/ask8.dat'   using 1:3 with lines linewidth 2.0 linecolor rgb '#6677zz' title '8-ASK',\
    'data/modem-phase-error/apsk8.dat'  using 1:3 with lines linewidth 2.0 linecolor rgb '#ccc55c' title '8-APSK',\
    'data/modem-phase-error/qam8.dat'   using 1:3 with lines linewidth 2.0 linecolor rgb '#009977' title '8-QAM',\
    'data/modem-phase-error/psk8.dat'   using 1:3 with lines linewidth 2.0 linecolor rgb '#770088' title '8-PSK'
