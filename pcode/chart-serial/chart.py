# file    : chart2.py
# author  : rb
# purpose : plot drawing test
# date    : 220804
# last    : 220831

# imports
from PyQt5 import QtGui
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QSize
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QTextEdit 
from PyQt5.QtGui import QPalette, QFont, QColor
from pyqtgraph.Qt import QtGui, QtCore, QtWidgets
import pyqtgraph as pg
from numpy import *
import time

# max number of displayed samples 
MAX_LEN = 200

# initialize Qt
app = QtWidgets.QApplication ([])

# test
class ChartWindow (QMainWindow):
  def __init__(self):
    super().__init__()  

    # x-axis startpoint
    self.tptr = -MAX_LEN
    self.sptr = -MAX_LEN
    self.vptr = -MAX_LEN

    # temperature datapoints 
    self.t0 = linspace (0, 0, num=MAX_LEN)
    self.t1 = linspace (0, 0, num=MAX_LEN)
    self.t2 = linspace (0, 0, num=MAX_LEN)
    self.t3 = linspace (0, 0, num=MAX_LEN)

    # soil moisture datapoints 
    self.s0 = linspace (0, 0, num=MAX_LEN)
    self.s1 = linspace (0, 0, num=MAX_LEN)
    self.s2 = linspace (0, 0, num=MAX_LEN)

    # baord voltages datapoints 
    self.v0 = linspace (0, 0, num=MAX_LEN)
    self.v1 = linspace (0, 0, num=MAX_LEN)
    self.v2 = linspace (0, 0, num=MAX_LEN)

    # set pen colors
    self.t0_pen = pg.mkPen (color=(224, 220, 153), width=1)  # shades of white
    self.t1_pen = pg.mkPen (color=(222, 215,  90), width=1)  # shades of white
    self.t2_pen = pg.mkPen (color=(180, 173,  55), width=1)  # shades of white
    self.t3_pen = pg.mkPen (color=(242,  54,  12), width=1)  # dark red

    self.s0_pen = pg.mkPen (color=( 30, 235,  45), width=1)  # shades of green
    self.s1_pen = pg.mkPen (color=( 26, 202,  38), width=1)  # shades of green
    self.s2_pen = pg.mkPen (color=( 18, 166,  29), width=1)  # shades of green

    self.v0_pen = pg.mkPen (color=( 50,  50, 255), width=1)  # Vdd
    self.v1_pen = pg.mkPen (color=(255, 255,   0), width=1)  # Vsol
    self.v2_pen = pg.mkPen (color=( 255,  0,   0), width=1)  # Vbat

    ## left section GUI
    # define wireless link status chart
    self.p_xdat = range(1, 32)  # x-axis bargraph
    self.p_ydat = [0]*32        # y-axis bargraph

    self.p_plot = pg.PlotWidget (title="- Packet Stats -")
    self.p_axis = self.p_plot.getAxis ('bottom')    #get x axis
    self.p_axis.setTickSpacing (1, 1)               #set x ticks (major and minor)
    self.p_barg = pg.BarGraphItem (x=self.p_xdat, height=self.p_ydat, width=0.4, brush="#eee8aa")
    self.p_plot.addItem (self.p_barg)
    self.p_plot.setBackground ((44, 44, 44))
    self.p_plot.enableAutoRange (axis='y', enable=True)
   #self.p_plot.setYRange (0.0, 20.0, padding=0.05)
    self.p_plot.setXRange (0, 32, padding=0.01)
    self.p_plot.showGrid (x=True, y=True, alpha=0.1)
    self.p_plot.setLabel ("left"  , "number" , units='-')
    self.p_plot.setLabel ("bottom", "retries", units='-')

    # define data packet dump window
    self.d_plot = QtWidgets.QTextEdit (self)
    self.d_plot.setText ("MOISTER LOG DATA")
    # set colors
    self.d_plot.setTextBackgroundColor (QtGui.QColor (54, 54, 54))
    self.d_plot.setTextColor (QtGui.QColor (250, 235, 215))
    self.d_plot.setStyleSheet ("QTextEdit {background-color:#363636;}")
    # set read-only
    self.d_plot.setReadOnly (True)
    # set font
    self.d_font = QtGui.QFont ()
    self.d_font.setPointSize (7)
    self.d_plot.setFont (self.d_font)
    # no scroll bars
    self.d_plot.setVerticalScrollBarPolicy (Qt.ScrollBarAlwaysOff); 

    ## right section GUI
    # define temperature chart
    self.t_plot = pg.PlotWidget (title="- Temperatures -")
    self.t_plot.setBackground ((44, 44, 44))
    self.t_plot.setYRange (20.0, 27.0, padding=0.05)
    self.t_plot.showGrid (x=True, y=True, alpha=0.3)
    self.t_plot.setLabel ("left"  , "temperature", units="oC")
    self.t_plot.setLabel ("bottom", "time"       , units='s')
    self.t_plot.addLegend (offset=1, frame=False, colCount=4, horSpacing=20)

    # define soil moisture chart
    self.s_plot = pg.PlotWidget (title="- Soil Moisture -")
    self.s_plot.setBackground ((44, 44, 44))
    self.s_plot.setYRange (0.0, 3000.0, padding=0.05)
    self.s_plot.showGrid (x=True, y=True, alpha=0.3)
    self.s_plot.setLabel ("left"  , "soil moisture", units='-')
    self.s_plot.setLabel ("bottom", "time"         , units='s')
    self.s_plot.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)

    # define board voltages chart
    self.v_plot = pg.PlotWidget (title="- Board Voltages -")
    self.v_plot.setBackground ((44, 44, 44))
    self.v_plot.setYRange (0.0, 4.0, padding=0.05)
    self.v_plot.showGrid (x=True, y=True, alpha=0.3)
    self.v_plot.setLabel ("left"  , "voltage", units='-')
    self.v_plot.setLabel ("bottom", "time"   , units='s')
    self.v_plot.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)

    # get curves in temperature plot, add name for Legend
    self.t_curve0 = self.t_plot.plot (name="Ttop")
    self.t_curve1 = self.t_plot.plot (name="Tmid")
    self.t_curve2 = self.t_plot.plot (name="Tbot")
    self.t_curve3 = self.t_plot.plot (name="Tamb")

    # get curves in soil moisture plot
    self.s_curve0 = self.s_plot.plot (name="Stop")
    self.s_curve1 = self.s_plot.plot (name="Smid")
    self.s_curve2 = self.s_plot.plot (name="Sbot")

    # get curves in board voltage plot
    self.v_curve0 = self.v_plot.plot (name="Vdda")
    self.v_curve1 = self.v_plot.plot (name="Vsol")
    self.v_curve2 = self.v_plot.plot (name="Vbat")

    ## build layout
    # define a top-level widget to hold everything
    self.win = pg.GraphicsLayoutWidget (title="- real-time MOISTER data -")
    self.win.setBackground ("#484837")
    self.win.resize (1600, 800)

    # define grid layout
    self.layout = QtWidgets.QGridLayout (self.win) 

    ## create layout to manage the widgets size and position
    # left part GUI
    self.layout.addWidget (self.p_plot, 0, 0, 1, 1)
    self.layout.addWidget (self.d_plot, 1, 0, 2, 1)

    # right part GUI
    self.layout.addWidget (self.t_plot, 0, 1, 1, 1)
    self.layout.addWidget (self.s_plot, 1, 1, 1, 1)
    self.layout.addWidget (self.v_plot, 2, 1, 1, 1)

    # stretch: left part small, right part wide
    self.layout.setColumnStretch (0, 30)
    self.layout.setColumnStretch (1, 70)

    # display the widget as a new window
    self.win.show()

  ## update packet counter
  def adjust_counter (self, cnt):    
   #self.button.setText ("PACKET COUNT: %d" % (cnt))
    pass

  ## update temperature data
  def update_temp (self, t):    
    # shift old data 
    self.t0[:-1] = self.t0[1:]
    self.t1[:-1] = self.t1[1:]
    self.t2[:-1] = self.t2[1:]
    self.t3[:-1] = self.t3[1:]

    # & add new temperature
    self.t0[-1] = t[0]
    self.t1[-1] = t[1]
    self.t2[-1] = t[2]
    self.t3[-1] = t[3]

    # update curve data
    self.t_curve0.setData (self.t0, pen=self.t0_pen)
    self.t_curve1.setData (self.t1, pen=self.t1_pen)
    self.t_curve2.setData (self.t2, pen=self.t2_pen)
    self.t_curve3.setData (self.t3, pen=self.t3_pen)
  
    # set curve x position to 0                    
    self.t_curve0.setPos (self.tptr, 0)                  
    self.t_curve1.setPos (self.tptr, 0)                  
    self.t_curve2.setPos (self.tptr, 0)                  
    self.t_curve3.setPos (self.tptr, 0)                  

    # update x position for displaying the curve
    self.tptr += 1                              

    # update event loop
    QtGui.QApplication.processEvents ()

  ## update soil moisture data
  def update_soil (self, s):    
    # shift old data 
    self.s0[:-1] = self.s0[1:]
    self.s1[:-1] = self.s1[1:]
    self.s2[:-1] = self.s2[1:]

    # & add new temperature
    self.s0[-1] = s[0]
    self.s1[-1] = s[1]
    self.s2[-1] = s[2]

    # update curve data
    self.s_curve0.setData (self.s0, pen=self.s0_pen)
    self.s_curve1.setData (self.s1, pen=self.s1_pen)
    self.s_curve2.setData (self.s2, pen=self.s2_pen)
  
    # set curve x position to 0     
    self.s_curve0.setPos (self.sptr, 0)                  
    self.s_curve1.setPos (self.sptr, 0)                  
    self.s_curve2.setPos (self.sptr, 0)                  

    # update x position for displaying the curve
    self.sptr += 1                              

    # update event loop
    QtGui.QApplication.processEvents ()

  ## update board voltage data
  def update_volt (self, v):    
    # shift old data 
    self.v0[:-1] = self.v0[1:]
    self.v1[:-1] = self.v1[1:]
    self.v2[:-1] = self.v2[1:]

    # & add new temperature
    self.v0[-1] = v[0]
    self.v1[-1] = v[1]
    self.v2[-1] = v[2]

    # update curve data
    self.v_curve0.setData (self.v0, pen=self.v0_pen)
    self.v_curve1.setData (self.v1, pen=self.v1_pen)
    self.v_curve2.setData (self.v2, pen=self.v2_pen)
  
    # set curve x position to 0
    self.v_curve0.setPos (self.vptr, 0)                  
    self.v_curve1.setPos (self.vptr, 0)                  
    self.v_curve2.setPos (self.vptr, 0)                  

    # update x position for displaying the curve
    self.vptr += 1                              

    # update event loop
    QtGui.QApplication.processEvents ()

  ## dump log data
  def update_logdata (self, txt):
    log = ""
    cnt = 0

    # ???
    tmp  = (' '.join(txt.split ()))
    tmp2 = tmp.split ()

    # get first data only (do not copy link status)
    for str in tmp2:
      if (cnt < 17):
        log = log + str + ' '
        cnt += 1

    # dump log data
    self.d_plot.append (log)
   #print (log)

  ## plot link status histogram
  def update_linkstatus (self, y):
    for i in self.p_xdat:
      self.p_ydat[i-1] = y[i-1]

    # update bargraph
    self.p_barg.setOpts ()

    # update event loop
    QtGui.QApplication.processEvents ()

# start the Qt event loop <> use 'QtGui.QApplication.processEvents ()' instead
#app.exec ()
#sys.exit(App.exec()) # wat is deze?



