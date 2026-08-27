# file    : client.py
# author  : rb
# purpose : Windows TCP client, connects to TCP server running on NFT MASTER 
# date    : 230314
# last    : 230619

# -- imports
import sys
import time
import socket

from PyQt5.QtWidgets import QWidget # test

from PyQt5.QtCore import pyqtSignal, pyqtSlot
from threading import Thread 
#import nft_chart              

# -- defines
HOST = "192.168.178.47"           # NFT ESP32 IP adr
PORT = 23                         # Telnet

# create initial log file
now = time.localtime ()
fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec))
f = open (fname, 'w')

err_cnt = 0                       # packet error counter
exp_cnt = 0                       # socket exception counter

# TCP client timer thread
class TimerThread (Thread):    # - org
  def __init__ (self, window):  # why 'window' here? runs also without
    Thread.__init__(self)
    self.setDaemon (True)       # set to Deamon for nice exit
    self.window = window

  def run (self): 
    global f

    while (True):
      # get current time & date
      now = time.localtime ()
	  
      # update wallclock
      nft_time (time.asctime (now), self.window)
			  
      # get new sample from NFT MASTER
      if (now.tm_sec == 0):                  # every 1 minute
        # convert to pretty format
        time_stamp = time.asctime (now)
       
        # get data from TCP server 
        nft_stat (time_stamp, self.window)

      # create new log file every day
      if ((now.tm_hour == 23) & (now.tm_min == 59) & (now.tm_sec == 59)):
        # send Signal for screen grab
       #fname = ("%4d%02d%02d-%02d%02d%02d.png" % (now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec))
        self.window.sendSig ()

        # close old logfile & open new one
        f.close ()
        fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec))
        f = open (fname, 'w')

        print ("#create new file %s" % (fname))

        # start over with new charts
        nft_reset (self.window)

      time.sleep (1)

# get log data from ESP32 TCP server & write to log file
def nft_stat (tnow, win):
  global err_cnt
  global exp_cnt

  # set up socket & connect to TCP server
  try:
    # instantiate socket & configure socket 
    c = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    c.settimeout (30)
    c.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    c.connect ((HOST, PORT))  

    # send 'stat' command
    c.send ('\n'.encode())             # flush possible spurs in ESP32 'recv' buffer
    c.send ("stat\n".encode())
  
    # read response
    response = c.recv(1024).decode("UTF-8")  

    # dump timestamp & data
    print (tnow, ' ', end='')
    print (response, end='')  

    # write to logfile
    f.write (tnow) 
    f.write (' ') 
    f.write (response[0:-1])           # supress empty line in log file
                                       
    # enter ESP32 power save           
    c.send ('\n'.encode())             # flush possible spurs in ESP32 'recv' buffer
    c.send ("esp_stop 50\n".encode())  # stop ESP32 for 50 seconds

    # parse response 
    win.update_state (response, err_cnt)

  except socket.timeout:
    err_cnt += 1
    print (f"#client socket timeout exception {err_cnt}")
    f.write (f"#socket timeout exeption {err_cnt}\n")
   #f.write (old_response[0:-1])       # <> todo

  except socket.error:
    exp_cnt += 1
    print (f"#some socket error (exp_cnt)")
    f.write (f"#some socket exeption {exp_cnt}\n")
	
  finally:
    c.close ()
    f.flush ()

# turn NFT MASTER OLED display on
def nft_oled ():
  # set up socket & connect to TCP server
  try:
    # instantiate socket & configure socket 
    c = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    c.settimeout (30)
    c.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    c.connect ((HOST, PORT))  

    # send 'oled_on' command
    c.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    c.send ("oled_on\n".encode())
  
  except socket.timeout:
    print ("#client socket timeout exeption")

  finally:
    c.close ()


# display actual time in NFT dashboard
def nft_time (tnow, win):
  win.update_time (tnow)


# reset chart pointers
def nft_reset (win):
  win.reset_charts ()



