set terminal windows

set grid

set style line 1 lt 2 lc rgb "black" lw 0.01

set style line 2 linewidth 1
set style line 3 linewidth 1
set style line 4 linewidth 1

set multiplot layout 2, 1 title "- Coco Coir Rinse pH/EC Effect- 190328-rb -" font ",11"
set tmargin 0.0

set style histogram cluster gap 1
set boxwidth 0.8 absolute
set style fill solid 1.00 border lt -1

set style data histogram

#unset ylabel

# ---- plot 1 ----
set xrange [0: 4]
set xtics 0, 1

set yrange [0: 8.1]
set ytics 0, 1
set ylabel 'pH [-]' rotate by 90

set key off
#set key bottom
#set key right

plot 'coco-pH_EC.dat' u 2  t 'pH'

# ---- plot 2 ----
set xrange [0: 4]
set xtics 0, 1

set yrange [0: 4.01]
set ytics 0, 0.5
set ylabel 'EC [mS]'  rotate by 90

set key off
#set key bottom
#set key right

plot 'coco-pH_EC.dat' u 3  t 'EC'

unset multiplot
