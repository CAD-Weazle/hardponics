set terminal windows

set title '- SimSun Ligth Intensity - With Reflectors - Kipp & Zonen PAR Sensor (4.57 umol/m^2.s) - 190125-rb -'

set grid
set xtics 0, 10
set ytics 0, 100
set xlabel 'PWM [%]'
set ylabel 'PAR [uV]'

plot 'LED-PAR_reflectors.dat' u 1:3 t 'Deep Red' w lp,   \
     ''                       u 2:4 t 'Royal Blue' w lp, \
     ''                       u 1:5 t 'both colors' w lp    