# data/fft_spgram_time.gnu

reset
set terminal postscript eps enhanced color solid rounded
#set xrange [0:200];
set yrange [-1.3:1.3]
set size ratio 0.6
set xlabel 'Sample Index'
set ylabel 'Time Series'
set key top right nobox
set grid xtics ytics
set pointsize 0.6
set grid linetype 1 linecolor rgb '#cccccc' lw 1
plot 'figures.gen/fft_spgram_time.dat' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '#004080' title 'real',\
     'figures.gen/fft_spgram_time.dat' using 1:3 with lines linetype 1 linewidth 3 linecolor rgb '#cccccc' title 'imag'
