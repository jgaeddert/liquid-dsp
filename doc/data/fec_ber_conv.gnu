#
# fec_ber_conv.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [-10:10]
set yrange [1e-5:1]
set logscale y
set format y '10^{%L}'
set size ratio 0.6
set xlabel 'SNR [dB]'
set ylabel 'BER'
set key bottom right nobox
set grid xtics ytics
set pointsize 0.6
set key bottom left nobox

set pointsize 0.5

set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot \
    'data/ber/ber_none.dat'     using 1:3 with linespoints pointtype 6 linecolor rgb '#666666' title 'Uncoded',\
    'data/ber/ber_v27.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#008040' title 'conv. r1/2, K=7',\
    'data/ber/ber_v29.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#007040' title 'conv. r1/2, K=9',\
    'data/ber/ber_v39.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#006040' title 'conv. r1/3, K=9',\
    'data/ber/ber_v615.dat'     using 1:3 with linespoints pointtype 6 linecolor rgb '#005040' title 'conv. r1/6, K=15'
