# file    : rb-example.py
# author  : rb
# purpose : PyQtGraph test code
# date    : 220831
# last    : 220831

import numpy as np

import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtWidgets

# define a top-level widget to hold everything
win = pg.GraphicsLayoutWidget (title="- real-time data from serial port -")
win.setBackground ((22, 22, 22))
win.resize (1600, 800)

layout = QtWidgets.QGridLayout (win)

## left part of display
p_xdat = range(1, 32)  # x-axis bargraph
p_ydat = [0]*32        # y-axis bargraph

p_plot = pg.PlotWidget (title="- Packet Stats -")
p_axis = p_plot.getAxis ('bottom')    #get x axis
p_axis.setTickSpacing (1, 1)               #set x ticks (major and minor)
p_barg = pg.BarGraphItem (x=p_xdat, height=p_ydat, width=0.4, brush="#eee8aa")
p_plot.addItem (p_barg)
p_plot.setBackground ((44, 44, 44))
p_plot.enableAutoRange (axis='y', enable=True)
#p_plot.setYRange (0.0, 20.0, padding=0.05)
p_plot.setXRange (0, 32, padding=0.01)
p_plot.showGrid (x=True, y=True, alpha=0.1)
p_plot.setLabel ("left"  , "number" , units='-')
p_plot.setLabel ("bottom", "retries", units='-')
p_plot.addLegend ()

#p_policy = p_plot.sizePolicy ()
#p_policy.setVerticalStretch (2)
#p_plot.setSizePolicy (p_policy)
#

# define data packet contents 
d_plot = QtWidgets.QTextEdit ()
d_plot.setTextBackgroundColor (QtGui.QColor (54, 54, 54))
d_plot.setTextColor (QtGui.QColor (250, 235, 215))
d_plot.setStyleSheet ("QTextEdit {background-color:#363636;}")
d_plot.setReadOnly (True)
d_plot.setText ("MOISTER LOG DATA")
d_font = QtGui.QFont ()
d_font.setPointSize (6)
d_plot.setFont (d_font)




## right part of display
# define temperature chart
#t_plot = pg.PlotWidget (title="- Temperatures -")

t_plot = pg.plot()
t_plot.setWindowTitle('pyqtgraph example: Legend')
t_plot.addLegend()

   
# define soil moisture chart
s_plot = pg.PlotWidget (title="- Soil Moisture -")

# define board voltages chart
v_plot = pg.PlotWidget (title="- Board Voltages -")


# build up layout
layout.addWidget (p_plot, 0, 0, 1, 1)
layout.addWidget (d_plot, 1, 0, 2, 1)

layout.addWidget (t_plot, 0, 1, 1, 1)
layout.addWidget (s_plot, 1, 1, 1, 1)
layout.addWidget (v_plot, 2, 1, 1, 1)

layout.setColumnStretch (0, 30)
layout.setColumnStretch (1, 70)


t_plot.plot([1,3,2,4,3,5], pen='r', name="test")


# display the widget as a new window
win.show ()

# run
if __name__ == "__main__":
  pg.exec ()




#app = pg.mkQApp ("Gradiant Layout Example")
#view = pg.GraphicsView ()
#
#layout = pg.GraphicsLayout (border=(100,100,100))
#view.setCentralItem (layout)
#
#view.show ()
#view.setWindowTitle ("- MOISTER -")
#view.setBackground ((44, 40, 44))
#view.resize (1200, 800)
#
#
### Title at top
#layout.addLabel ("MOISTER realt-time logging", col=0, row=0, rowspan=1, colspan=10)
#layout.nextRow ()
#
### histogram & log data windows
#l1 = layout.addLayout (rowspan=1, colspan=2, border=(44,40,44))
##l1.setContentsMargins (10, 10, 10, 10)
#p21 = l1.addPlot (row=0, col=0, colspan=1, rowspan=1, border=(44,40,44))
##vb = p21.getViewBox ()
##vb.setBackgroundColor ((44, 44, 44))
#p22 = l1.addPlot (row=1, col=0, colspan=1, rowspan=1, border=(44,40,44))
##vb = p22.getViewBox ()
##vb.setBackgroundColor ((44, 44, 44))
#
#
### add 3 plots into the first colums (automatic position)
#l2 = layout.addLayout (rowspan=3, colspan=4, border=(44,40,44))
#
#p1 = l2.addPlot (row=0, col=1, colspan=4, title="- Temperatures -", border=(44,40,44))
#vb = p1.getViewBox ()
#vb.setBackgroundColor ((44, 44, 44))
#p1.setYRange (20.0, 27.0, padding=0.05)
#p1.showGrid (x=True, y=True, alpha=0.3)
#p1.setLabel ("left"  , "temperature", units="oC")
##p1.setLabel ("bottom", "time"       , units='s')
#p1.addLegend ()
##layout.nextRow ()
#
#p2 = l2.addPlot (row=1, col=1, colspan=4, title="- Soil Moisture -")
#vb = p2.getViewBox ()
#vb.setBackgroundColor ((44, 44, 44))
#p2.setYRange (0.0, 3000.0, padding=0.05)
#p2.showGrid (x=True, y=True, alpha=0.3)
#p2.setLabel ("left"  , "soil moisture", units='-')
##p2.setLabel ("bottom", "time"         , units='s')
#p2.addLegend ()
#
#layout.nextRow ()
#p3 = l2.addPlot (row=2, col=1, colspan=4, title="- Board Voltages -")
#vb = p3.getViewBox ()
#vb.setBackgroundColor ((44, 44, 44))
#p3.setYRange (0.0, 4.0, padding=0.05)
#p3.showGrid (x=True, y=True, alpha=0.3)
#p3.setLabel ("left"  , "voltage", units='-')
#p3.setLabel ("bottom", "time"       , units='s')
#p3.addLegend ()
#
### hide axes on top plots
##p1.hideAxis ('bottom')
##p2.hideAxis ('bottom')
#
### show some content in the plots
#p1.plot([21,23,22,24,23,25])
#p2.plot([1000,3000,300,400,300,0])
#p3.plot([1,2,4,3,4,1])
##p4.plot([1,3,2,4,3,5])
##p5.plot([1,3,2,4,3,5])
#
#view.show ()
#
#
#
## run
#if __name__ == "__main__":
#  pg.exec ()
#






#glay = QtWidgets.QGridLayout ()
#glay.addWidget(lineedit, 0, 0)
#glay.addWidget(button2, 0, 2)
#
#glay.addWidget(widget, 2, 0, 1, 3)  
#
#glay.addWidget(button, 4, 0)                                    
#glay.addWidget(button1, 4, 2)
#
#glay.setColumnStretch(1, 1)                                     # setColumnStretch
#glay.setRowStretch(1, 1)                                        # setRowStretch
#glay.setRowStretch(2, 2)                                        # setRowStretch
#glay.setRowStretch(3, 1)                                        # setRowStretch
#
