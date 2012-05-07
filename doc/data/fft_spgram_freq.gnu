# data/fft_spgram_time.gnu

reset
set terminal postscript eps enhanced color solid rounded
set xrange [-0.5:0.5]
set yrange [0:60]
set size ratio 0.6
set xlabel 'Normalized Frequency'
set ylabel 'Time'
set nokey
set view map
set palette defined (-100 "white", -50 "white", -30 "#ACA1A4", 0 "#400040")
splot 'figures.gen/fft_spgram_freq.dat' using 2:1:3 with image
