#
# fec_ber_convpunc.gnu
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
    'data/ber/ber_v27.dat'      using 1:3 with linespoints pointtype 6 linecolor rgb '#004080' title 'conv. r1/2, K=7',\
    'data/ber/ber_v27p23.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004070' title 'conv. r2/3, K=7',\
    'data/ber/ber_v27p34.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004060' title 'conv. r3/4, K=7',\
    'data/ber/ber_v27p45.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004050' title 'conv. r4/5, K=7',\
    'data/ber/ber_v27p56.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004040' title 'conv. r5/6, K=7',\
    'data/ber/ber_v27p67.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004030' title 'conv. r6/7, K=7',\
    'data/ber/ber_v27p78.dat'   using 1:3 with linespoints pointtype 6 linecolor rgb '#004020' title 'conv. r7/8, K=7'
