# Import libraries
import serial
from numpy import *
from pyqtgraph.Qt import QtGui, QtCore, QtWidgets
import pyqtgraph as pg
import time
#import sport    # own source 'sport.py'
import threading

# globals
sstr = ""
snew = 0
scnt = 0
slst = ""

# open serial port
ser = serial.Serial ('COM4', 9600)  

### START QtApp #####
# you MUST do this once (initialize things)
app = QtGui.QApplication([])             

# creates a window
win = pg.GraphicsWindow (title="- real-time data from serial port -") 

# creates empty space for the plot in the window
plot1 = win.addPlot (title="Realtime plot")  

# create an empty "plot" (a curve to plot)
curve = plot1.plot ()                        

# setup chart
plot1.showGrid (x=True,y=True)
plot1.addLegend ()
plot1.setLabel ("left"  , "Temperature", units='oC')
plot1.setLabel ("bottom", "Time" , units='s')
#plot1.setXRange (0, 20)
plot1.setYRange (20.0, 30.0)
#???.setTickSpacing (1, 0.1)
plot1.setWindowTitle ('- Test PyQtGraph Plot -')

windowWidth = 500                        # width of the window displaying the curve
Xm = linspace (0,0,windowWidth)          # create array that will contain the relevant time series     
ptr = -windowWidth                       # set first x position

# realtime data plot
def update ():
  global curve, ptr, Xm, slst    

  time.sleep (0.01)

  temp = float(slst[4])
  humi = float(slst[6])

  # shift samples & add newest
  Xm[:-1] = Xm[1:]                      
  Xm[-1] = temp              

  # update x position for displaying the curve
  ptr += 1                              

  # set the curve with this data
  curve.setData (Xm)

  # set x position in the graph to 0                    
  curve.setPos (ptr,0)                  

  # process plot
  QtGui.QApplication.processEvents()    


# setup serial port
def serial_open (port, baud):
  # set port number & Baudrate
  ser.port = port
  ser.baudrate = int(baud)

  # open port
  ser.open ()

  # print port info
  print ("serial port: %s, %d" % (ser.name, ser.baudrate))              

# close serial port
def serial_close ():
  ser.close ()  

# get number characters in input buffer
def sdata_inwaiting ():
  return (ser.in_waiting)

# read data from serial port - threaded
def sdata_read ():
  global sstr
  global snew

  while True:
    sdat = ser.readline ()
    
    # decode to string
    sstr = sdat.decode ()
    snew = 1

# handle serial data
def sdata_parse (sstr):
  # remove double spaces & '\n' from string (???)
  istr = ' '.join(sstr.split ())
  
  # split to list
  ilst = istr.split (" ")

  return (ilst)

# dump serial data
def sdata_dump (ilst): 
  print ("temperature: %2.1f" % (float(ilst[4])))
  print ("humidity   : %2.1f" % (float(ilst[6])))

# --- main ---

# define Thread - use 'deamon' so Thread can be stopped
threading.Thread (target=sdata_read, daemon=True).start ()

# --- main ---
while True:
  # add blocking element to allow for task switch
  time.sleep (0.01)

  if (snew):
    slst = sdata_parse (sstr)
    sdata_dump (slst)
    update ()
    snew = 0

# end QtApp
pg.QtGui.QApplication.exec_()



