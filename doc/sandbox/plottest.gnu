# figures.gen/modem_16qam.gnu : auto-generated file (do not edit)
# invoked as : ./src/modem.genplot -f figures.gen/modem_16qam.gnu -g 4.2 -t eps -d figures.gen/modem_16qam.dat
reset
#set terminal postscript eps enhanced color solid rounded
set terminal png rounded small size 540,480 enhanced crop
set xrange [-1.5:1.5]
set yrange [-1.5:1.5]
set size square
set title "constellation"
set xlabel "I"
set ylabel "Q"
set nokey
set grid xtics ytics lc rgb '#999999' lw 1
set pointsize 1.0
xoffset = 0.06
yoffset = 0.06
plot 'plottest.dat' using 1:2 with points pointtype 7 linecolor rgb '#004080',   \
     'plottest.dat' using ($1+xoffset):($2+yoffset):4 with labels
