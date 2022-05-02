set title "Realtime plotting of Encoder"
set xlabel "Time"
set ylabel "Count"
plot 'minicom.dat' u 1  with lines lt rgb "red"
pause 0.1
reread
set autoscale

