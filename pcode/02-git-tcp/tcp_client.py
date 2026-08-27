# file    : tcp_client.py
# author  : rb
# purpose : Windows TCP client, connects to TCP server running on NFT MASTER 
# date    : 230314
# last    : 230314

# -- imports
import sys
import time
import socket

from threading import Thread 
#import nft_chart              

# -- defines
HOST = "192.168.178.47"           # NFT ESP32 IP adr
PORT = 23                         # Telnet

# TCP client timer thread
class TimerThread (Thread):    # - org
  def __init__ (self, window):  # why 'window' here? runs also without
    Thread.__init__(self) 
    self.window = window
  
  def run (self): 
    while (True):
      # get current time & date
      now = time.localtime ()
      
      # get new sample from NFT MASTER
     #if ((now.tm_min%5 == 0) & (now.tm_min == 0)):  # every 5 minutes
      if ((now.tm_sec%10 == 0)):                     # every 10 seconds <> test
        # convert to pretty format
        time_stamp = time.asctime (now)
        
        # get data from TCP server 
        nft_stat (time_stamp, self.window)

      time.sleep (1)

# get log data from ESP32 TCP server & write to log file
def nft_stat (tnow, win):
  # set up socket & connect to TCP server
  try:
    # instantiate socket & configure socket 
    c = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    c.settimeout (30)
    c.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    c.connect ((HOST, PORT))  

    # send 'stat' command
    c.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    c.send ("stat\n".encode())
  
    # read response
    response = c.recv(1024).decode("UTF-8")  

    # dump timestamp
    print (tnow, ' ', end='')
    
    # & dump
    print (response, end='')  
	
    # parse response 
    win.update_state (response)

  except socket.timeout:
    print ("#client socket timeout exception")

  finally:
    c.close ()


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

