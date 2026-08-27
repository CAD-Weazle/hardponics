# file    : chart.py
# author  : rb
# purpose : NFT dashboard GUI
# date    : 230314
# last    : 230514

# logdata format: Wed Feb 15 20:35:20 2023 >CNT: 1751 VBAT: 11.47 IBAT: -52.35 VSOL: 5.65 T: 14.26 RH: 60.65 PAR: 1776 T_EC: -22.67 EC: 0.34
# chart1: Vbat + Vsol
# chart2: Ibat
# chart3: T + RH + PAR (normalized 0..100 <> todo)

# imports
import sys
import time

from numpy     import *
from pathlib   import Path
from client    import TimerThread
from threading import Thread 

import pyqtgraph as pg

from PyQt5.QtCore import pyqtSignal, pyqtSlot

from pyqtgraph.Qt    import QtGui, QtCore, QtWidgets
from PyQt5           import QtGui
from PyQt5           import QtCore
from PyQt5.QtCore    import QCoreApplication
from PyQt5.QtCore    import Qt, QSize, pyqtSlot
from PyQt5.QtGui     import QIcon, QPalette, QFont, QColor
from PyQt5.QtWidgets import QScrollBar,QSplitter,QTableWidgetItem,QTableWidget
from PyQt5.QtWidgets import QDialog
from PyQt5.QtWidgets import QAction, qApp
from PyQt5.QtWidgets import QFileDialog, QMessageBox, QToolBar, QStatusBar
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QVBoxLayout
from PyQt5.QtWidgets import QHBoxLayout
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QTextEdit
from PyQt5.QtWidgets import QWidget 
from PyQt5.QtWidgets import QMenuBar

# number of chart points
MAX_LEN = 1440                         # number of samples for one day

# board voltages chart
class VoltageChart (pg.PlotWidget):
  def __init__(self):
    super().__init__()
    self.setTitle ("- Board Voltages -")
    self.setBackground ((44, 44, 44))
    self.setXRange (0, MAX_LEN, padding=0)
    self.setYRange (0.0, 25.0, padding=0.05)
    self.showGrid (x=True, y=True, alpha=0.3)
    self.setLabel ("left"  , "voltage", units='V')
    self.setLabel ("bottom", "time"   , units='s')
    self.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)
    self.setDownsampling (ds=None, auto=None, mode="subsample")

# battery current chart
class CurrentChart (pg.PlotWidget):
  def __init__(self):
    super().__init__()
    self.setTitle ("- Battery Current -")
    self.setBackground ((44, 44, 44))
    self.setXRange (0, MAX_LEN, padding=0)
    self.setYRange (-500.0, 1000.0, padding=0.05)
    self.showGrid (x=True, y=True, alpha=0.3)
    self.setLabel ("left"  , "current", units='A')
    self.setLabel ("bottom", "time"   , units='s')
    self.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)
    self.setDownsampling (ds=None, auto=None, mode="subsample")

# temperature/humidity chart
class TempChart (pg.PlotWidget):
  def __init__(self):
    super().__init__()
    self.setTitle ("- Temperature, Humidity & PAR -")
    self.setBackground ((44, 44, 44))
    self.setXRange (0, MAX_LEN, padding=0)
    self.setYRange (0.0, 100.0, padding=0.05)
    self.showGrid (x=True, y=True, alpha=0.3)
    self.setLabel ("left"  , "temperature/humidity", units="oC/%RH/PAR")
    self.setLabel ("bottom", "time"                , units='s')
    self.addLegend (offset=1, frame=False, colCount=4, horSpacing=20)
    self.setDownsampling (ds=None, auto=None, mode="subsample")

# data packet dump window
class DumpWindow (QtWidgets.QTextEdit):
  def __init__(self):
    super().__init__()
    self.setTextBackgroundColor (QtGui.QColor (54, 54, 54))
    self.setTextColor (QtGui.QColor (250, 235, 215))
    self.setStyleSheet ("QTextEdit {background-color:#363636;}")
    self.setReadOnly (True)
    font = QtGui.QFont ()
    font.setPointSize (7)
    self.setFont (font)

# NFT MASTER dashboard GUI
class myWindow (QMainWindow):
  # define Signal for screen grab
  sigGrab = pyqtSignal ()

  def __init__(self):
    super().__init__()

    # connect to screen grabbing function
    self.sigGrab.connect (self.scrGrab)

    # file extension filter
    self.filters = 'NFT Log Files (*.log)'
    self.path = None

    # x-axis startpoint
    self.tptr = 0                                               # temperature pointer
    self.vptr = 0                                               # voltages pointer
    self.iptr = 0                                               # current point

    # define datapoints 
    self.vb = linspace (0, 0, num=MAX_LEN)                      # Vbat
    self.vs = linspace (0, 0, num=MAX_LEN)                      # Vsol
    self.ib = linspace (0, 0, num=MAX_LEN)                      # Ibat
    self.t  = linspace (0, 0, num=MAX_LEN)                      # temperature
    self.h  = linspace (0, 0, num=MAX_LEN)                      # humidity
    self.p  = linspace (0, 0, num=MAX_LEN)                      # sun intensity PAR

    for i in range (0, MAX_LEN):
      self.vb[i] = -10
      self.vs[i] = -10
      self.ib[i] = -800
      self.t[i]  = -10
      self.h[i]  = -10
      self.p[i]  = -10
                                                                
    # define pen colors                                         
    self.vb_pen = pg.mkPen (color=( 43, 242,  50), width=1)     # Vbat
    self.vs_pen = pg.mkPen (color=(242, 250, 128), width=1)     # Vsol
    self.ib_pen = pg.mkPen (color=(201,   3,   3), width=1)     # Ibat
    self.t_pen  = pg.mkPen (color=(201,  39,   3), width=1)     # temperature
    self.h_pen  = pg.mkPen (color=(  0, 205, 239), width=1)     # humidity
    self.p_pen  = pg.mkPen (color=(242, 250, 128), width=1)     # sun intensity PAR

    TAG_WIDTH = 325
    VAL_WIDTH = 125
    UNI_WIDTH = 60
    BOT_WIDT1 = 325
    BOT_WIDT2 = 190
    BOT_HEIGT = 30

    ## left section GUI
    # define status section
    self.vbat_tag = QLabel (f" battery voltage")           # tag label (static)
    self.vbat_val = QLabel (f"0.00 ")                      # value label (dynamic)
    self.vbat_uni = QLabel (f"V ")                         # unit label (static)
    self.vbat_tag.setFixedWidth (TAG_WIDTH)
    self.vbat_val.setFixedWidth (VAL_WIDTH)
    self.vbat_uni.setFixedWidth (UNI_WIDTH)
    self.vbat_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.vbat_val.setAlignment (QtCore.Qt.AlignRight)
    self.vbat_uni.setAlignment (QtCore.Qt.AlignRight)
    self.vsol_tag = QLabel (f" solar voltage")             # tag label (static)
    self.vsol_val = QLabel (f"0.00 ")                      # value label (dynamic)
    self.vsol_uni = QLabel (f"V ")                         # unit label (static)
    self.vsol_tag.setFixedWidth (TAG_WIDTH)
    self.vsol_val.setFixedWidth (VAL_WIDTH)
    self.vsol_uni.setFixedWidth (UNI_WIDTH)
    self.vsol_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.vsol_val.setAlignment (QtCore.Qt.AlignRight)
    self.vsol_uni.setAlignment (QtCore.Qt.AlignRight)

    self.ibat_tag = QLabel (f" battery current")           # tag label (static)
    self.ibat_val = QLabel (f"0.00 ")                      # value label (dynamic)
    self.ibat_uni = QLabel (f"mA ")                        # unit label (static)
    self.ibat_tag.setFixedWidth (TAG_WIDTH)
    self.ibat_val.setFixedWidth (VAL_WIDTH)
    self.ibat_uni.setFixedWidth (UNI_WIDTH)
    self.ibat_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.ibat_val.setAlignment (QtCore.Qt.AlignRight)
    self.ibat_uni.setAlignment (QtCore.Qt.AlignRight)

    self.pcnt_tag = QLabel (f" packet")                    # tag label (static)
    self.pcnt_val = QLabel (f"0 ")                         # value label (dynamic)
    self.pcnt_uni = QLabel (f"")                           # unit label (static)
    self.pcnt_tag.setFixedWidth (TAG_WIDTH)
    self.pcnt_val.setFixedWidth (VAL_WIDTH)
    self.pcnt_val.setFixedWidth (BOT_WIDT2)
    self.pcnt_val.setFixedHeight (BOT_HEIGT)
    self.pcnt_uni.setFixedWidth (UNI_WIDTH)
    self.pcnt_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.pcnt_val.setAlignment (QtCore.Qt.AlignRight)
    self.pcnt_uni.setAlignment (QtCore.Qt.AlignRight)

    self.temp_tag = QLabel (f" temperature")               # tag label (static)
    self.temp_val = QLabel (f"0.00 ")                      # value label (dynamic)
    self.temp_uni = QLabel (f"\u2103 ")                    # unit label (static)
    self.temp_tag.setFixedWidth (TAG_WIDTH)
    self.temp_val.setFixedWidth (VAL_WIDTH)
    self.temp_uni.setFixedWidth (UNI_WIDTH)
    self.temp_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.temp_val.setAlignment (QtCore.Qt.AlignRight)
    self.temp_uni.setAlignment (QtCore.Qt.AlignRight)

    self.humi_tag = QLabel (f" humidity")                  # tag label (static)
    self.humi_val = QLabel (f"0.00 ")                      # value label (dynamic)
    self.humi_uni = QLabel (f"%RH ")                       # unit label (static)
    self.humi_tag.setFixedWidth (TAG_WIDTH)
    self.humi_val.setFixedWidth (VAL_WIDTH)
    self.humi_uni.setFixedWidth (UNI_WIDTH)
    self.humi_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.humi_val.setAlignment (QtCore.Qt.AlignRight)
    self.humi_uni.setAlignment (QtCore.Qt.AlignRight)

    self.par_tag = QLabel (f" PAR")                        # tag label (static)
    self.par_val = QLabel (f"0 ")                          # value label (dynamic)
    self.par_uni = QLabel (f"- ")                          # unit label (static)
    self.par_tag.setFixedWidth (TAG_WIDTH)
    self.par_val.setFixedWidth (VAL_WIDTH)
    self.par_uni.setFixedWidth (UNI_WIDTH)
    self.par_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.par_val.setAlignment (QtCore.Qt.AlignRight)
    self.par_uni.setAlignment (QtCore.Qt.AlignRight)

    self.ec_tag = QLabel (f" EC")                          # tag label (static)
    self.ec_val = QLabel (f"0.00 ")                        # value label (dynamic)
    self.ec_uni = QLabel (f"mS ")                          # unit label (static)
    self.ec_tag.setFixedWidth (TAG_WIDTH)
    self.ec_val.setFixedWidth (VAL_WIDTH)
    self.ec_uni.setFixedWidth (UNI_WIDTH)
    self.ec_tag.setAlignment (QtCore.Qt.AlignLeft)
    self.ec_val.setAlignment (QtCore.Qt.AlignRight)
    self.ec_uni.setAlignment (QtCore.Qt.AlignRight)

    self.timestamp = QLabel (f"")                          # value label (dynamic)
    self.timestamp.setAlignment (QtCore.Qt.AlignLeft)
    self.timestamp.setFixedWidth (BOT_WIDT1)
    self.timestamp.setFixedHeight (BOT_HEIGT)

    self.vbat_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.vbat_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.vbat_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.vsol_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.vsol_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.vsol_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ibat_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ibat_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ibat_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.pcnt_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.pcnt_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.pcnt_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.temp_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.temp_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.temp_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.humi_tag.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.humi_val.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.humi_uni.setStyleSheet  ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.par_tag.setStyleSheet   ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.par_val.setStyleSheet   ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.par_uni.setStyleSheet   ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ec_tag.setStyleSheet    ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ec_val.setStyleSheet    ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.ec_uni.setStyleSheet    ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.timestamp.setStyleSheet ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")

    s_font = QtGui.QFont ()
    s_font.setPointSize (15)
    self.vsol_tag.setFont  (s_font)
    self.vsol_val.setFont  (s_font)
    self.vsol_uni.setFont  (s_font)
    self.vbat_tag.setFont  (s_font)
    self.vbat_val.setFont  (s_font)
    self.vbat_uni.setFont  (s_font)
    self.ibat_tag.setFont  (s_font)
    self.ibat_val.setFont  (s_font)
    self.ibat_uni.setFont  (s_font)
    self.pcnt_tag.setFont  (s_font)
    self.pcnt_val.setFont  (s_font)
    self.pcnt_uni.setFont  (s_font)
    self.temp_tag.setFont  (s_font)
    self.temp_val.setFont  (s_font)
    self.temp_uni.setFont  (s_font)
    self.humi_tag.setFont  (s_font)
    self.humi_val.setFont  (s_font)
    self.humi_uni.setFont  (s_font)
    self.par_tag.setFont   (s_font)
    self.par_val.setFont   (s_font)
    self.par_uni.setFont   (s_font)
    self.ec_tag.setFont    (s_font)
    self.ec_val.setFont    (s_font)
    self.ec_uni.setFont    (s_font)
    self.timestamp.setFont (s_font)

    # layout parameter tags (left column)
    self.st_plot = QVBoxLayout ()
    self.st_plot.addWidget (self.vsol_tag)
    self.st_plot.addWidget (self.vbat_tag)
    self.st_plot.addWidget (self.ibat_tag)
   #self.st_plot.addWidget (self.pcnt_tag) # do not display packet counter
    self.st_plot.addWidget (self.temp_tag)
    self.st_plot.addWidget (self.humi_tag)
    self.st_plot.addWidget (self.par_tag)
    self.st_plot.addWidget (self.ec_tag)

    # layout parameter dynamic values (mid column)
    self.sv_plot = QVBoxLayout ()
    self.sv_plot.addWidget (self.vsol_val)
    self.sv_plot.addWidget (self.vbat_val)
    self.sv_plot.addWidget (self.ibat_val)
    self.sv_plot.addWidget (self.temp_val)
    self.sv_plot.addWidget (self.humi_val)
    self.sv_plot.addWidget (self.par_val)
    self.sv_plot.addWidget (self.ec_val)

    # layout parameter units (right column)
    self.su_plot = QVBoxLayout ()
    self.su_plot.addWidget (self.vsol_uni)
    self.su_plot.addWidget (self.vbat_uni)
    self.su_plot.addWidget (self.ibat_uni)
    self.su_plot.addWidget (self.temp_uni)
    self.su_plot.addWidget (self.humi_uni)
    self.su_plot.addWidget (self.par_uni)
    self.su_plot.addWidget (self.ec_uni)
 
    # all columns in horizontal box
    self.sx_plot = QHBoxLayout ()
    self.sx_plot.addLayout (self.st_plot)
    self.sx_plot.addLayout (self.sv_plot)
    self.sx_plot.addLayout (self.su_plot)

    # timestamp & packet counter
    self.sy_plot = QHBoxLayout ()
    self.sy_plot.addWidget (self.timestamp)
    self.sy_plot.addWidget (self.pcnt_val)

    # add to parameter section
    self.s_plot = QVBoxLayout ()
    self.s_plot.addLayout (self.sx_plot)
    self.s_plot.addLayout (self.sy_plot)

    # define data packet dump window
    self.d_plot = DumpWindow ()

    ## right section GUI
    # define board voltages chart
    self.v_plot = VoltageChart ()
    self.vb_curve = self.v_plot.plot (name="Vbat")
    self.vs_curve = self.v_plot.plot (name="Vsol")

    # define battery current chart
    self.i_plot = CurrentChart ()
    self.ib_curve = self.i_plot.plot (name="Ibat")

    # define temperature & humidity chart
    self.t_plot = TempChart ()
    self.t_curve  = self.t_plot.plot (name="T")
    self.h_curve  = self.t_plot.plot (name="%RH")
    self.p_curve  = self.t_plot.plot (name="PAR")

    ## build layout
    # define a top-level widget to hold everything
    self.win = pg.GraphicsLayoutWidget (title="- real-time NFT data -")
    self.win.setMinimumSize (1600, 800)
    self.win.setBackground ("#484837")
    self.setCentralWidget (self.win) 
 
    # define grid layout
    self.layout = QtWidgets.QGridLayout (self.win) 

    ## create layout to manage the widgets size and position
    # left part GUI
    self.layout.addLayout (self.s_plot, 0, 0, 1, 1)
    self.layout.addWidget (self.d_plot, 1, 0, 2, 1)

    # right part GUI
    self.layout.addWidget (self.v_plot, 0, 1, 1, 1)
    self.layout.addWidget (self.i_plot, 1, 1, 1, 1)
    self.layout.addWidget (self.t_plot, 2, 1, 1, 1)

    # stretch: left part small, right part wide
    self.layout.setColumnStretch (0, 30)
    self.layout.setColumnStretch (1, 70)

    # init menubar & statusbar
    self.initUI ()
    self.setStatusBar (QStatusBar(self))

    # display the widget as a new window
    self.win.show()


  # emit Signal (called in timer Thread)
  def sendSig (self):
    self.sigGrab.emit ()

  # grab screen
  def scrGrab (self):
    # get date
    now = time.localtime ()
    fname = ("%4d%02d%02d.png" % (now.tm_year, now.tm_mon, now.tm_mday))

    # grab screen & save
    pix = self.win.grab ()
    pix.save (fname)

  ## update board voltage data
  def update_volt (self, vb, vs):    
    # add new datapoints
    self.vb[self.vptr] = vb
    self.vs[self.vptr] = vs

    # update curve data
    self.vb_curve.setData (self.vb, pen=self.vb_pen)
    self.vs_curve.setData (self.vs, pen=self.vs_pen)
  
    # update x position for displaying the curve
    if (self.vptr < MAX_LEN-1):
      self.vptr += 1                              

  ## update battery current data
  def update_current (self, ib):    
    # add new datapoint
    self.ib[self.iptr] = ib
  
    # update curve data
    self.ib_curve.setData (self.ib, pen=self.ib_pen)
  
    # update x position for displaying the curve
    if (self.iptr < MAX_LEN-1):
      self.iptr += 1                              

  ## update temperature & humidity data
  def update_humitemp (self, t, h, p):    
    # add new datapoint
    self.t[self.tptr] = t
    self.h[self.tptr] = h

    # convert to int, scale & back to string...
    p_int = int(p)
   #self.p[self.tptr] = str(p_int/25)   # <> scale beter later - too low
   #self.p[self.tptr] = str(p_int/15)   # <> too low (max = 80)
    self.p[self.tptr] = str(p_int/12.5) # <> busy

    # update curve data
    self.t_curve.setData (self.t, pen=self.t_pen)
    self.h_curve.setData (self.h, pen=self.h_pen)
    self.p_curve.setData (self.p, pen=self.p_pen)

    # update x position for displaying the curve
    if (self.tptr < MAX_LEN-1):
      self.tptr += 1                              

  ## dump log data
  def update_logdata (self, txt):
    log = ""
    cnt = 0

    # ???
    tmp  = (' '.join(txt.split ()))
    tmp2 = tmp.split ()

    # get first data only (do not copy link status)
    for str in tmp2:
      if (cnt < 22):
        log = log + str + ' '
        cnt += 1

    # dump log data
    self.d_plot.append (log)
    self.d_plot.ensureCursorVisible ()           # add line to bottom, scroll text up

  ## plot current NFT status
  def update_boardstatus (self, cnt, err, vb, ib, vs, temp, humi, par, ec):
    self.vbat_val.setText (f"{vb} ")
    self.vsol_val.setText (f"{vs} ")
    self.ibat_val.setText (f"{ib} ")
    self.pcnt_val.setText (f"{cnt}/{err} ")
    self.temp_val.setText (f"{temp} ")
    self.humi_val.setText (f"{humi} ")
    self.par_val.setText  (f"{par} ")
    self.ec_val.setText   (f"{ec} ")

  ## plot current time
  def update_time (self, tnow):
    self.timestamp.setText   (f" {tnow}")
  
  ## reset charts
  def reset_charts (self):
    # reset chart pointers 
    self.vptr = 0         # voltages pointer
    self.iptr = 0         # current point
    self.tptr = 0         # temperature pointer

    for i in range (0, MAX_LEN):
      self.vb[i] = -10
      self.vs[i] = -10
      self.ib[i] = -800
      self.t[i]  = -10
      self.h[i]  = -10
      self.p[i]  = -10

  # define menubar
  def initUI (self):
    # define menu item: open log file
    openAction = QAction ('Open logfile', self)
    openAction.setShortcut ('Ctrl+O')
    openAction.setStatusTip ('Read log file')
    openAction.triggered.connect (self.open_file)

    # define menu item: exit
    exitAction = QAction ('Exit', self)
    exitAction.setShortcut ('Ctrl+Q')
    exitAction.setStatusTip ('Exit application')
    exitAction.triggered.connect (qApp.quit)

    # define menu bar at top of window holding above menu
    menubar = self.menuBar ()
    menubar.setNativeMenuBar (False)
    filemenu = menubar.addMenu ('&File')
    filemenu.addAction (openAction)
    filemenu.addAction (exitAction)

    # define status bar (info at bottom of window)
    self.status_bar = self.statusBar ()
    self.status_bar.showMessage ('Ready', 1000)              # display message for 1 second

  def update_state (self, resp, errcnt):
    # remove double spaces & '\n' from string (noice, but why???)
    tmp = ' '.join(resp.split ())

    # split to list
    lst = list (tmp.split(" "))
  
    try:
      nft_pcnt = lst[1]
      nft_vbat = lst[3]
      nft_ibat = lst[5]
      nft_vsol = lst[7]
      nft_temp = lst[9]
      nft_humi = lst[11]
      nft_par  = lst[13]
      nft_ec   = lst[17]
     #nft_ec_temp = lst[15] # not used	  
    except:
      print ("some input error - A")
   
    # update chart
    self.update_volt (nft_vbat, nft_vsol)
    self.update_current (nft_ibat)
    self.update_humitemp (nft_temp, nft_humi, nft_par)
    self.update_logdata (resp)
    self.update_boardstatus (nft_pcnt, errcnt, nft_vbat, nft_ibat, nft_vsol, nft_temp, nft_humi, nft_par, nft_ec)

  # open logfile
  def open_file (self):
    filename, _ = QFileDialog.getOpenFileName (self, "./logfiles/", filter=self.filters)

    if filename:
      self.path = Path (filename)                               # get full path
      name = self.path.name                                     # get filename only
     #self.status_bar.showMessage (f"Reading {name}", 3000)     # show at bottom of window <> werkt niet

      f = open (self.path, "r+")

      # reset chart pointers 
      self.vptr = 0         # voltages pointer
      self.iptr = 0         # current point
      self.tptr = 0         # temperature pointer

    while True:
      line = f.readline()
    
      # remove double spaces & '\n' from string (noice, but why???)
      tmp = ' '.join(line.split ())
    
      # split to list
      lst = list (tmp.split(" "))
    
      if (not line):
        break
    
      try:
        nft_pcnt = lst[6]
        nft_vbat = lst[8]
        nft_ibat = lst[10]
        nft_vsol = lst[12]
        nft_temp = lst[14]
        nft_humi = lst[16]
        nft_par  = lst[18]
        nft_ec   = lst[20]
      except:
        print ("some input error - B")
    
      # update chart
      self.update_volt (nft_vbat, nft_vsol)
      self.update_current (nft_ibat)
      self.update_humitemp (nft_temp, nft_humi, nft_par)
     #self.update_logdata (line)
      self.update_boardstatus (nft_pcnt, 0, nft_vbat, nft_ibat, nft_vsol, nft_temp, nft_humi, nft_par, nft_ec, "")

    f.close()



