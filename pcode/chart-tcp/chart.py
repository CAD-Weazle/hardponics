# file    : chart.py
# author  : rb
# purpose : NFT dashboard chart
# date    : 220804
# last    : 230220

# logdata format: Wed Feb 15 20:35:20 2023 >CNT: 1751 VBAT: 11.47 IBAT: -52.35 VSOL: 5.65 T: 14.26 RH: 60.65 PAR: 1776 T_EC: -22.67 EC: 0.34
# Vbat + Vsol
# Ibat
# T + RH + PAR

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
from threading import Thread 

tcpClient = None

# max number of displayed samples 
MAX_LEN = 200

# initialize Qt
app = QtWidgets.QApplication ([])

# test
class ChartWindow (QMainWindow):
  def __init__(self):
    super().__init__()  

    # x-axis startpoint
    self.tptr = -MAX_LEN  # temperature pointer
    self.hptr = -MAX_LEN  # humidity pointer
    self.sptr = -MAX_LEN
    self.vptr = -MAX_LEN

    # define datapoints 
    self.vb = linspace (0, 0, num=MAX_LEN)                      # Vbat
    self.vs = linspace (0, 0, num=MAX_LEN)                      # Vsol
    self.ib = linspace (0, 0, num=MAX_LEN)                      # Ibat
    self.t  = linspace (0, 0, num=MAX_LEN)                      # temperature
    self.h  = linspace (0, 0, num=MAX_LEN)                      # humidity
    self.p  = linspace (0, 0, num=MAX_LEN)                      # sun intensity PAR
                                                                
    # define pen colors                                         
    self.vb_pen = pg.mkPen (color=( 43, 242,  50), width=1)     # Vbat
    self.vs_pen = pg.mkPen (color=(242, 250, 128), width=1)     # Vsol
    self.ib_pen = pg.mkPen (color=(201,   3,   3), width=1)     # Ibat
    self.t_pen  = pg.mkPen (color=(201,   3,   3), width=1)     # temperature
    self.h_pen  = pg.mkPen (color=(203, 205, 239), width=1)     # humidity
    self.p_pen  = pg.mkPen (color=(242, 250, 128), width=1)     # sun intensity PAR

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
    self.d_plot.setText ("NFT LOG DATA")
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
    self.d_plot.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff); 

    ## right section GUI
    # define board voltages/current chart
    self.v_plot = pg.PlotWidget (title="- Board Voltages -")
    self.v_plot.setBackground ((44, 44, 44))
    self.v_plot.setYRange (0.0, 20.0, padding=0.05)
    self.v_plot.showGrid (x=True, y=True, alpha=0.3)
    self.v_plot.setLabel ("left"  , "voltage", units='-')
    self.v_plot.setLabel ("bottom", "time"   , units='s')
    self.v_plot.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)

    # define battery current chart
    self.i_plot = pg.PlotWidget (title="- Battery Current -")
    self.i_plot.setBackground ((44, 44, 44))
    self.i_plot.setYRange (-500.0, 1000.0, padding=0.05)
    self.i_plot.showGrid (x=True, y=True, alpha=0.3)
    self.i_plot.setLabel ("left"  , "curretn", units='A')
    self.i_plot.setLabel ("bottom", "time"   , units='s')
    self.i_plot.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)

    # define temperature & humidity chart
    self.t_plot = pg.PlotWidget (title="- Temperature & Humidity -")
    self.t_plot.setBackground ((44, 44, 44))
    self.t_plot.setYRange (0.0, 100.0, padding=0.05)
    self.t_plot.showGrid (x=True, y=True, alpha=0.3)
    self.t_plot.setLabel ("left"  , "temperature/humidity", units="oC/%RH")
    self.t_plot.setLabel ("bottom", "time"       , units='s')
    self.t_plot.addLegend (offset=1, frame=False, colCount=4, horSpacing=20)

    # define traces
    self.vb_curve = self.v_plot.plot (name="Vbat")
    self.vs_curve = self.v_plot.plot (name="Vsol")
    self.ib_curve = self.i_plot.plot (name="Ibat")
    self.t_curve  = self.t_plot.plot (name="T")
    self.h_curve  = self.t_plot.plot (name="%RH")

    ## build layout
    # define a top-level widget to hold everything
    self.win = pg.GraphicsLayoutWidget (title="- real-time NFT data -")
    self.win.setBackground ("#484837")
    self.win.resize (1600, 800)

    # define grid layout
    self.layout = QtWidgets.QGridLayout (self.win) 

    ## create layout to manage the widgets size and position
    # left part GUI
    self.layout.addWidget (self.p_plot, 0, 0, 1, 1)
    self.layout.addWidget (self.d_plot, 1, 0, 2, 1)

    # right part GUI
    self.layout.addWidget (self.v_plot, 0, 1, 1, 1)
    self.layout.addWidget (self.i_plot, 1, 1, 1, 1)
    self.layout.addWidget (self.t_plot, 2, 1, 1, 1)

    # stretch: left part small, right part wide
    self.layout.setColumnStretch (0, 30)
    self.layout.setColumnStretch (1, 70)

    # display the widget as a new window
    self.win.show()

# ## update packet counter
# def adjust_counter (self, cnt):    
#  #self.button.setText ("PACKET COUNT: %d" % (cnt))
#   pass

  ## get data from NFT server
  def tcp_send (self):
    tcpClient.send ("\n".encode())
    tcpClient.send ("stat\n".encode())

  ## update temperature data
  def update_temp (self, t):    
    # shift old data 
    self.t[:-1] = self.t[1:]

    # & add new temperature
    self.t[-1] = t

    # update curve data
    self.t_curve.setData (self.t, pen=self.t_pen)

    # set curve x position to 0                    
    self.t_curve.setPos (self.tptr, 0)                  

    # update x position for displaying the curve
    self.tptr += 1                              

    # update event loop
    QtGui.QApplication.processEvents ()

  ## update humidity data
  def update_humi (self, h):    
    # shift old data 
    self.h[:-1] = self.h[1:]

    # & add new humidity sample
    self.h[-1] = h

    # update curve data
    self.h_curve.setData (self.h, pen=self.h_pen)

    # set curve x position to 0                    
    self.h_curve.setPos (self.hptr, 0)                  

    # update x position for displaying the curve
    self.hptr += 1                              

    # update event loop
    QtGui.QApplication.processEvents ()


# ## update soil moisture data
# def update_soil (self, s):    
#   # shift old data 
#   self.s0[:-1] = self.s0[1:]
#   self.s1[:-1] = self.s1[1:]
#   self.s2[:-1] = self.s2[1:]
#
#   # & add new temperature
#   self.s0[-1] = s[0]
#   self.s1[-1] = s[1]
#   self.s2[-1] = s[2]
#
#   # update curve data
#   self.s_curve0.setData (self.s0, pen=self.s0_pen)
#   self.s_curve1.setData (self.s1, pen=self.s1_pen)
#   self.s_curve2.setData (self.s2, pen=self.s2_pen)
# 
#   # set curve x position to 0     
#   self.s_curve0.setPos (self.sptr, 0)                  
#   self.s_curve1.setPos (self.sptr, 0)                  
#   self.s_curve2.setPos (self.sptr, 0)                  
#
#   # update x position for displaying the curve
#   self.sptr += 1                              
#
#   # update event loop
#   QtGui.QApplication.processEvents ()
#
  ## update board voltage data
  def update_volt (self, vb, vs, ib):    
    # shift old data 
    self.vb[:-1] = self.vb[1:]
    self.vs[:-1] = self.vs[1:]
    self.ib[:-1] = self.ib[1:]
  
    # & add new temperature
    self.vb[-1] = vb
    self.vs[-1] = vs
    self.ib[-1] = ib
  
    # update curve data
    self.vb_curve.setData (self.vb, pen=self.vb_pen)
    self.vs_curve.setData (self.vs, pen=self.vs_pen)
    self.ib_curve.setData (self.ib, pen=self.ib_pen)
  
    # set curve x position to 0
    self.vb_curve.setPos (self.vptr, 0)                  
    self.vs_curve.setPos (self.vptr, 0)                  
    self.ib_curve.setPos (self.vptr, 0)                  
  
    # update x position for displaying the curve
    self.vptr += 1                              
  
    # update event loop
    QtGui.QApplication.processEvents ()
  
# ## dump log data
# def update_logdata (self, txt):
#   log = ""
#   cnt = 0
#
#   # ???
#   tmp  = (' '.join(txt.split ()))
#   tmp2 = tmp.split ()
#
#   # get first data only (do not copy link status)
#   for str in tmp2:
#     if (cnt < 17):
#       log = log + str + ' '
#       cnt += 1
#
#   # dump log data
#   self.d_plot.append (log)
#  #print (log)
#
# ## plot link status histogram
# def update_linkstatus (self, y):
#   for i in self.p_xdat:
#     self.p_ydat[i-1] = y[i-1]
#
#   # update bargraph
#   self.p_barg.setOpts ()
#
#   # update event loop
#   QtGui.QApplication.processEvents ()

# start the Qt event loop <> use 'QtGui.QApplication.processEvents ()' instead
#app.exec ()
#sys.exit(App.exec()) # wat is deze?




   #self.s0_pen = pg.mkPen (color=( 30, 235,  45), width=1)  # shades of green
   #self.s1_pen = pg.mkPen (color=( 26, 202,  38), width=1)  # shades of green
   #self.s2_pen = pg.mkPen (color=( 18, 166,  29), width=1)  # shades of green
