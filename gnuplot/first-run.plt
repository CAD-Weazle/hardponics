set terminal windows

set title '- GroBox Control - First Pepper Plant - 190201-rb -'

set grid
#set xtics 0, 10
set ytics 0, 10
set xlabel 'time [s]'
set ylabel '[-]'
set yrange [0: 80]

plot 'first-run.log' u  2 t 'LED_RED_box1' w l,  \
     'first-run.log' u  3 t 'LED_BLUE_box1' w l, \
     'first-run.log' u  5 t 'Temp_led1' w l,     \
     'first-run.log' u  7 t 'Temp_box1' w l,     \
     'first-run.log' u 12 t 'LED_RED_box2' w l,  \
     'first-run.log' u 13 t 'LED_BLUE_box2' w l, \
     'first-run.log' u 15 t 'Temp_led2' w l,     \
     'first-run.log' u 17 t 'Temp_box2' w l,     \
     'first-run.log' u 22 t 'LED_RED_box3' w l,  \
     'first-run.log' u 23 t 'LED_BLUE_box3' w l, \
     'first-run.log' u 25 t 'Temp_led3' w l,     \
     'first-run.log' u 27 t 'Temp_box3' w l
