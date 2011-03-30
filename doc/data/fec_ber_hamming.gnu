#
# fec_ber_hamming.gnu
#
reset
set terminal postscript eps enhanced color solid rounded
set xrange [-5:10]
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
    'data/ber/ber_h128.dat'     using 1:3 with linespoints pointtype 6 linecolor rgb '#004080' title 'Hamming(12,8)',\
    'data/ber/ber_h74.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#008040' title 'Hamming(7,4)',\
    'data/ber/ber_r3.dat'       using 1:3 with linespoints pointtype 6 linecolor rgb '#400040' title 'Repeat(3)',\
    'data/ber/ber_r5.dat'       using 1:3 with linespoints pointtype 6 linecolor rgb '#800080' title 'Repeat(5)'

#    'data/ber/ber_h84.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#008040' title 'Hamming(8,4)'
