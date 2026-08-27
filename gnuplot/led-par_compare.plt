set terminal windows

set title '- SimSun Ligth Intensity - Compare w/wo Reflectors - Kipp & Zonen PAR Sensor (4.57 uV/umol/m^2.s) - 190125-rb -'

set grid
set xtics 0, 10
set ytics 0, 500
set xlabel 'PWM [%]'
set ylabel 'PAR [uV]'

plot 'LED-PAR_no-reflectors.dat' u 1:3 t 'Deep Red   - wo reflectors' w lp,  \
     'LED-PAR_reflectors.dat'    u 1:3 t 'Deep Red   - w. reflectors' w lp,  \
     'LED-PAR_no-reflectors.dat' u 2:4 t 'Royal Blue - wo reflectors' w lp,  \
     'LED-PAR_reflectors.dat'    u 2:4 t 'Royal Blue - w. reflectors' w lp,  \
     'LED-PAR_no-reflectors.dat' u 1:5 t 'both colors - wo reflectors' w lp, \
     'LED-PAR_reflectors.dat'    u 1:5 t 'both colors - w. reflectors' w lp
