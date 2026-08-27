# from: https://github.com/anuranBarman/Python-Chat-Application-Using-PyQt-and-Socket/blob/master/client.py

import sys,time
import socket

from numpy import *

from PyQt5 import QtGui
from PyQt5 import QtCore
from PyQt5.QtWidgets import QScrollBar,QSplitter,QTableWidgetItem,QTableWidget,QComboBox,QGridLayout,QDialog,QWidget, QPushButton, QApplication, QMainWindow,QAction,QMessageBox,QLabel,QTextEdit,QProgressBar,QLineEdit
from PyQt5.QtCore import QCoreApplication
from pyqtgraph.Qt import QtGui, QtCore, QtWidgets

from threading import Thread 
#from socketserver import ThreadingMixIn 

from PyQt5 import QtGui
from PyQt5.QtWidgets import QApplication, QMainWindow, QAction, qApp
from PyQt5.QtWidgets import QTextEdit, QFileDialog, QMessageBox, QToolBar, QStatusBar, QLabel
from PyQt5.QtGui     import QIcon, QPalette, QFont, QColor
from PyQt5.QtCore    import Qt, QSize, pyqtSlot
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QVBoxLayout
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import QGridLayout
from PyQt5.QtWidgets import QTextEdit 
from PyQt5.QtWidgets import QWidget 

import pyqtgraph as pg

#tcpClientA = None
cnt = 0

# get log data from ESP32 TCP server & write to log file
def get_data ():
  host = "192.168.178.47"  # ESP32
  port = 23                # ESP32

  tnow = time.ctime ()
  
  try:
    # instantiate socket & configure socket 
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    s.settimeout (30)
    s.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    s.connect ((host, port))  

    # send 'stat' command
    s.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    s.send ("stat\n".encode())
  
    # read response
    response = s.recv(1024).decode("UTF-8")  

    # dump timestamp
    print (tnow, ' ', end='')

    # & dump
    print (response, end='')  
	
    # parse response 
    window.update_state (response)

  except socket.timeout:
    print ("#socket timeout exeption")

  finally:
    s.close ()

# number of chart points
MAX_LEN = 1440            # number of samples for one day

# define board voltages plot appearance 
def define_vplot (p):
  p.setBackground ((44, 44, 44))
  p.setXRange (0, MAX_LEN, padding=0)
  p.setYRange (0.0, 20.0, padding=0.05)
  p.showGrid (x=True, y=True, alpha=0.3)
  p.setLabel ("left"  , "voltage", units='V')
  p.setLabel ("bottom", "time"   , units='s')
  p.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)
  p.setDownsampling (ds=None, auto=None, mode="subsample")

# define battery current plot appearance 
def define_iplot (p):
  p.setBackground ((44, 44, 44))
  p.setXRange (0, MAX_LEN, padding=0)
  p.setYRange (-500.0, 1000.0, padding=0.05)
  p.showGrid (x=True, y=True, alpha=0.3)
  p.setLabel ("left"  , "current", units='A')
  p.setLabel ("bottom", "time"   , units='s')
  p.addLegend (offset=1, frame=False, colCount=3, horSpacing=20)
  p.setDownsampling (ds=None, auto=None, mode="subsample")

# define temperature/humidity plot appearance 
def define_tplot (p):
  p.setBackground ((44, 44, 44))
  p.setXRange (0, MAX_LEN, padding=0)
  p.setYRange (0.0, 100.0, padding=0.05)
  p.showGrid (x=True, y=True, alpha=0.3)
  p.setLabel ("left"  , "temperature/humidity", units="oC/%RH")
  p.setLabel ("bottom", "time"                , units='s')
  p.addLegend (offset=1, frame=False, colCount=4, horSpacing=20)
  p.setDownsampling (ds=None, auto=None, mode="subsample")


class Window (QMainWindow):       # rb  - test
  def __init__(self):
    super().__init__()

    # file extension filter
    self.filters = 'NFT Log Files (*.log)'
    self.path = None

    # x-axis startpoint
    self.tptr = 0         # temperature pointer
    self.vptr = 0         # voltages pointer
    self.iptr = 0         # current point

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
    self.t_pen  = pg.mkPen (color=(201,  39,   3), width=1)     # temperature
    self.h_pen  = pg.mkPen (color=(  0, 205, 239), width=1)     # humidity
    self.p_pen  = pg.mkPen (color=(242, 250, 128), width=1)     # sun intensity PAR

    ## left section GUI
    # define status section
    self.label1 = QLabel (f"  Vbat 0.0 V")      
    self.label2 = QLabel (f"  Vsol 0.0 V")      
    self.label3 = QLabel (f"  Ibat 0.0 mA")      
    self.label4 = QLabel (f"  packet 0")      

    self.label1.setStyleSheet ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.label2.setStyleSheet ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.label3.setStyleSheet ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")
    self.label4.setStyleSheet ("QLabel {background-color: rgb(54, 54, 54); color: rgb(250, 235, 215)}")

    s_font = QtGui.QFont ()
    s_font.setPointSize (20)
    self.label1.setFont (s_font)
    self.label2.setFont (s_font)
    self.label3.setFont (s_font)
    self.label4.setFont (s_font)

    self.s_plot = QVBoxLayout ()

    self.s_plot.addWidget (self.label1)
    self.s_plot.addWidget (self.label2)
    self.s_plot.addWidget (self.label3)
    self.s_plot.addWidget (self.label4)

    # define data packet dump window
    self.d_plot = QtWidgets.QTextEdit (self)
    self.d_plot.setText ("NFT LOG DATA")
    self.d_plot.setTextBackgroundColor (QtGui.QColor (54, 54, 54))
    self.d_plot.setTextColor (QtGui.QColor (250, 235, 215))
    self.d_plot.setStyleSheet ("QTextEdit {background-color:#363636;}")
    self.d_plot.setReadOnly (True)
    self.d_font = QtGui.QFont ()
    self.d_font.setPointSize (7)
    self.d_plot.setFont (self.d_font)
   #self.d_plot.setVerticalScrollBarPolicy (Qt.ScrollBarAlwaysOff);     # no scroll bars

    ## right section GUI
    # define board voltages chart
    self.v_plot = pg.PlotWidget (title="-  Board Voltages -")
    define_vplot (self.v_plot)
    self.vb_curve = self.v_plot.plot (name="Vbat")
    self.vs_curve = self.v_plot.plot (name="Vsol")

    # define battery current chart
    self.i_plot = pg.PlotWidget (title="- Battery Current -")
    define_iplot (self.i_plot)
    self.ib_curve = self.i_plot.plot (name="Ibat")

    # define temperature & humidity chart
    self.t_plot = pg.PlotWidget (title="- Temperature & Humidity -")
    define_tplot (self.t_plot)
    self.t_curve  = self.t_plot.plot (name="T")
    self.h_curve  = self.t_plot.plot (name="%RH")

    ## build layout
    # define a top-level widget to hold everything
    self.win = pg.GraphicsLayoutWidget (title="- real-time NFT data -")
    self.win.setBackground ("#484837")
    self.win.setMinimumSize (1600, 800)
 
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

  ## update packet counter
  def adjust_counter (self, cnt):    
   #self.button.setText ("PACKET COUNT: %d" % (cnt))
    pass

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

    # update event loop
   #QtGui.QApplication.processEvents ()     # werkt niet meer???
    QtGui.QGuiApplication.processEvents ()
  
  ## update battery current data
  def update_current (self, ib):    
    # add new datapoint
    self.ib[self.iptr] = ib
  
    # update curve data
    self.ib_curve.setData (self.ib, pen=self.ib_pen)
  
    # update x position for displaying the curve
    if (self.iptr < MAX_LEN-1):
      self.iptr += 1                              
  
    # update event loop
   #QtGui.QApplication.processEvents ()
    QtGui.QGuiApplication.processEvents ()

  ## update temperature & humidity data
  def update_humitemp (self, t, h):    
    # add new datapointd
    self.t[self.tptr] = t
    self.h[self.tptr] = h

    # update curve data
    self.t_curve.setData (self.t, pen=self.t_pen)
    self.h_curve.setData (self.h, pen=self.h_pen)

    # update x position for displaying the curve
    if (self.tptr < MAX_LEN-1):
      self.tptr += 1                              

    # update event loop
   #QtGui.QApplication.processEvents ()
    QtGui.QGuiApplication.processEvents ()

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

  ## plot current NFT status
  def update_boardstatus (self, vb, vs, ib, cnt):
    self.label1.setText (f"  Vbat {vb} V")
    self.label2.setText (f"  Vsol {vs} V")
    self.label3.setText (f"  Vbat {ib} mA")
    self.label4.setText (f"  packet {cnt} ")

    # update event loop
   #QtGui.QApplication.processEvents ()
    QtGui.QGuiApplication.processEvents ()

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

    # define menu bar at top of window holding above menu temes
    menubar = self.menuBar ()
    menubar.setNativeMenuBar (False)
    filemenu = menubar.addMenu ('&File')
    filemenu.addAction (openAction)
    filemenu.addAction (exitAction)

    # define status bar (info at bottom of window)
    self.status_bar = self.statusBar ()
    self.status_bar.showMessage ('Ready', 1000)              # display message for 1 second

  def update_state (self, resp):
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
      nft_ec   = lst[15]
    except:
      print ("some input error - A")
   
   #print ("packet: %d"  % int(nft_pcnt)  , end=' ')
   #print ("Vbat: %2.2f" % float(nft_vbat), end=' ')
   #print ("Ibat: %2.2f" % float(nft_ibat), end=' ')
   #print ("Vsol: %2.2f" % float(nft_vsol), end=' ')
   #print ("T: %2.2f"    % float(nft_temp), end=' ')
   #print ("RH: %2.2f"   % float(nft_humi), end=' ')
   #print ("PAR: %3.2f"  % float(nft_par) , end=' ')
   #print ("EC: %1.2f"   % float(nft_ec))
 
    # update chart
    self.update_volt (nft_vbat, nft_vsol)
    self.update_current (nft_ibat)
    self.update_humitemp (nft_temp, nft_humi)
    self.update_logdata (resp)
    self.update_boardstatus (nft_vbat, nft_vsol, nft_ibat, nft_pcnt)

  # open logfile
  def open_file (self):
    filename, _ = QFileDialog.getOpenFileName (self, "./logfiles/", filter=self.filters)

    if filename:
      self.path = Path (filename)                            # get full path
      name = self.path.name                                  # get filename only
     #self.status_bar.showMessage (f"Reading {name}", 3000)  # show at bottom of window <> werkt niet

      f = open (self.path, "r+")

      # reset plot pointers 
      self.tptr = 0         # temperature pointer
      self.vptr = 0         # voltages pointer
      self.iptr = 0         # current point

    while True:
      line = f.readline()
    
      # remove double spaces & '\n' from string (noice, but why???)
      tmp = ' '.join(line.split ())
    
      # split to list
      lst = list (tmp.split(" "))
    
      if (not line):
        break
     #print (line.strip())
    
      try:
        nft_pcnt = lst[6]
        nft_vbat = lst[8]
        nft_ibat = lst[10]
        nft_vsol = lst[12]
        nft_temp = lst[14]
        nft_humi = lst[16]
       #nft_par  = lst[18]
       #nft_ec   = lst[22]
      except:
        print ("some input error - B")
    
     #print ("packet: %d"  % int(nft_pcnt)  , end=' ')
     #print ("Vbat: %2.2f" % float(nft_vbat), end=' ')
     #print ("Ibat: %2.2f" % float(nft_ibat), end=' ')
     #print ("Vsol: %2.2f" % float(nft_vsol), end=' ')
     #print ("T: %2.2f"    % float(nft_temp), end=' ')
     #print ("RH: %2.2f"   % float(nft_humi), end=' ')
     #print ("PAR: %3.2f"  % float(nft_par) , end=' ')
     #print ("EC: %1.2f"   % float(nft_ec))
    
      # update chart
      self.update_volt (nft_vbat, nft_vsol)
      self.update_current (nft_ibat)
      self.update_humitemp (nft_temp, nft_humi)
      self.update_logdata (line)
      self.update_boardstatus (nft_vbat, nft_vsol, nft_ibat, nft_pcnt)

    f.close()

  def send (self, cmd): # obselete?
    tcpClientA.send ("stat\n".encode()) # rb


class TimerThread (Thread):
  def __init__(self,window): 
    Thread.__init__(self) 
    self.window = window
    self.cnt = 0
  
  def run (self): 
    while True:
     #print (self.cnt)
      self.cnt += 1

      if (self.cnt > 10):
        get_data ()
        self.cnt = 0

      time.sleep (1)


# -- main -----------------
app = QApplication ([])
window = Window ()
window.move (20, 20)
#window.show ()

timerThread = TimerThread (window)
timerThread.start ()

#window.exec ()

print ("bonjour")

app.exec ()


