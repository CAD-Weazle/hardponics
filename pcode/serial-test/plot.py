# file    : plot.py
# author  : rb
# purpose : PyQtChart test
# date    : 220722
# last    : 220722

import pyqtgraph.examples
import pyqtgraph as pg
#import pyqtgraph.exporters
import numpy as np
import sys
import random
import time
import sport    # own source 'sport.py'

pyqtgraph.examples.run()

# data set <> dev only, from serial port later
x = [0,1,2,3,4,5,6,7,8,9]
y = [2.0,4.0,6.0,8.0,10.0,12.0,14.0,16.0,18.0,20.0]

# define chart
plt = pg.plot ()

# define chart settings
def chart_define (): 
  # set chart properties
  plt.showGrid (x=True,y=True)
  plt.addLegend ()
  plt.setLabel ("left"  , "Value", units='V')
  plt.setLabel ("bottom", "Time" , units='s')
  plt.setXRange (0, 20)
  plt.setYRange (0, 30)
  plt.setWindowTitle ('- Test PyQtGraph Plot -')


# add datapoint to chart & update
def chart_update (y_new):
  # increment x-axis & add datapoint to y-axis
  x.append (x[-1] + 1)              
  y.append (y_new)                  

  # replot chart
  c1 = plt.plot (x, y, pen='b', symbol='x', symbolPen='b', symbolBrush=0.2)


# -- main --
# setup chart
chart_define ()

# add points <> test, from serial port later
for i in range(10):
  sdat = sport.sdata_read ()
  slst = sport.sdata_parse (sdat)
  temp = float(slst[4])
  print (temp)

  #plt.setXRange (0, i)
  chart_update (temp)

## start Qt event loop
pg.QtGui.QApplication.exec_()



























