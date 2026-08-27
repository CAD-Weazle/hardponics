set terminal windows

set title '- SimSun Total Photon Flux - Kipp & Zonen PAR Sensor (4.57 uV/umol/m^2.s) - 190125-rb -'

set grid
set xtics 0, 10
set ytics 0, 10
set xlabel 'PWM [%]'
set ylabel 'photon flux [mol/day]'

plot 'LED-PAR_reflectors.dat' u 1:(($3/4.57) * 60 * 60 *24)/10**6 t 'Deep Red   - w. reflectors' w lp,  \
     'LED-PAR_reflectors.dat' u 2:(($4/4.57) * 60 * 60 *24)/10**6 t 'Royal Blue - w. reflectors' w lp,  \
     'LED-PAR_reflectors.dat' u 1:(($5/4.57) * 60 * 60 *24)/10**6 t 'both colors - w. reflectors' w lp
