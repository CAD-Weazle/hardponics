# file    : main.py
# author  : rb
# purpose : NFT dashboard
# date    : 230220
# last    : 230313

# imports
import sys, time
import socket  
import datetime
from pyqtgraph.Qt import QtWidgets
from chart import ChartWindow
from threading import Thread 
from PyQt5.QtWidgets import QScrollBar,QSplitter,QTableWidgetItem,QTableWidget,QComboBox,QVBoxLayout,QGridLayout,QDialog,QWidget, QPushButton, QApplication, QMainWindow,QAction,QMessageBox,QLabel,QTextEdit,QProgressBar,QLineEdit

import chart

# ESP32 WIFI module 
host = "192.168.178.47"           # module IP address ESP32 module
port = 23                         # TELNET port number

# create initial log file
now = datetime.datetime.now ()
fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.year, now.month, now.day, now.hour, now.minute, now.second))
f = open (fname, 'w')

# get log data from ESP32 TCP server & write to log file
def get_data ():
  global f

  old_reseponse = ""

  tnow = time.ctime ()

  # get current time & date
  now = datetime.datetime.now ()
  
  # creat new log file every day
  if ((now.hour == 0) & (now.minute == 0) & (now.second == 0)):
    f.close ()
  
    fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.year, now.month, now.day, now.hour, now.minute, now.second))
  
    f = open (fname, 'w')
  
    print ("create new file %s" % (fname))
  
  # check sample interval
  if (now.second == 0):           # sample each minute 
    get_data ()
  
  # relax a bit
  time.sleep (1)
  
  try:
    # instantiate socket & configure socket 
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    s.settimeout (20)
    s.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    s.connect ((host, port))  

    # send 'stat' command
    s.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    s.send ("stat\n".encode())
  
    # read response
    response = s.recv(1024).decode("UTF-8")  

    # dump timestamp
   #print (time.ctime (), ' ', end='')
    print (tnow, ' ', end='')

   #f.write (time.ctime()) 
    f.write (tnow) 
    f.write (' ') 

    # & dump
    print (response, end='')  
    f.write (response[0:-1])      # supress empty line in log file

    # enter ESP32 power save
    s.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    s.send ("esp_stop 40\n".encode())
	
	# update state
    old_response = response
	
  except socket.timeout:
    print ("#socket timeout exeption")
    f.write ("#socket timeout exeption")
   #f.write (old_response[0:-1])  # supress empty line in log file

  finally:
    s.close ()
    f.flush ()


# ------- MAIN ----------- test
app = QApplication ([])
main = ChartWindow ()
main.move (20, 20)        # move to absolute screen coordinates
main.show ()

clientThread = chart.ClientThread (main)
clientThread.start ()

#main.exec ()
app.exec_()


# ------- MAIN ----------- org RB
#app = QtWidgets.QApplication ([])

## create main window & set screen location
#main = ChartWindow ()
#main.move (20, 20)        # move to absolute screen coordinates
#main.show ()
#
#clientThread = ClientThread (main)
#clientThread.start ()
#
#app.exec_ ()








## start TCP client
#while True:  
#  # get current time & date
#  now = datetime.datetime.now ()
#
#  # creat new log file every day
#  if ((now.hour == 0) & (now.minute == 0) & (now.second == 0)):
#    f.close ()
#
#    fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.year, now.month, now.day, now.hour, now.minute, now.second))
#
#    f = open (fname, 'w')
#
#    print ("create new file %s" % (fname))
#
#  # check sample interval
#  if (now.second == 0):           # sample each minute 
#    get_data ()
#
#  print ('.')
#
#  # relax a bit
#  time.sleep (1)
#
#f.close ()
