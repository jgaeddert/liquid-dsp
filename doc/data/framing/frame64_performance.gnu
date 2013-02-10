#
# frame64_performance.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [-12:6]
set yrange [1e-3:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.6
set size 1.0
set xlabel 'SNR [dB]'
set ylabel 'Probability of Missed Detection/Decoding'
set key bottom right nobox
set grid xtics ytics
set pointsize 0.6
#set key top right nobox
set key bottom left nobox

set pointsize 0.6

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot 'figures.gen/frame64_performance.dat' using 1:2 with linespoints linewidth 3 pointtype 7 linecolor rgb '#6060a0' title 'Frame Detection',\
     'figures.gen/frame64_performance.dat' using 1:4 with linespoints linewidth 3 pointtype 7 linecolor rgb '#202040' title 'Header/Payload Decoding'

