set title "Realtime plotting of Voltage"
set xlabel "Time"
set ylabel "Voltage"
plot 'minicom.dat' u 2 with lines lt rgb "red"
pause 0.001
reread
set autoscale

