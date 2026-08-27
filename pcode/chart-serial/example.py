"""
Demonstrates use of PlotWidget class. This is little more than a 
GraphicsView with a PlotItem placed in its center.
"""

import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets

app = pg.mkQApp ()
mw = QtWidgets.QMainWindow ()
mw.setWindowTitle ('pyqtgraph example: PlotWidget')
mw.resize (800,800)

cw = QtWidgets.QWidget ()
mw.setCentralWidget (cw)
l = QtWidgets.QVBoxLayout ()
cw.setLayout (l)

pw = pg.PlotWidget (name='Plot1')
l.addWidget(pw)

mw.show()

## create an empty plot curve to be filled later, set its pen
p1 = pw.plot ()
p1.setPen ((200,200,100))

pw.setLabel ('left', 'Value', units='V')
pw.setLabel ('bottom', 'Time', units='s')
pw.setXRange (0, 2)
pw.setYRange (0, 1e-10)

def rand(n):
  data = np.random.random(n)
  data[int(n*0.1):int(n*0.13)] += .5
  data[int(n*0.18)] += 2
  data[int(n*0.1):int(n*0.13)] *= 5
  data[int(n*0.18)] *= 20
  data *= 1e-12
  return data, np.arange(n, n+len(data)) / float(n)
    
def updateData():
  yd, xd = rand(10000)
  print (type(yd))
  p1.setData(y=yd, x=xd)

## Start a timer to rapidly update the plot in pw
t = QtCore.QTimer ()
t.timeout.connect (updateData)
t.start (50)
#updateData()

if __name__ == '__main__':
  pg.exec()
