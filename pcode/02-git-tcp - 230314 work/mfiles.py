# file    : client.py
# author  : rb
# purpose : TCP client - executed on Windows machine 
# date    : 220201
# last    : 230303
#

import socket  
import datetime
import time

# ESP32 WIFI module 
host = "192.168.178.47"           # module IP address ESP32 module
port = 23                         # TELNET port number

# create initial log file
now = datetime.datetime.now ()

# get log data from ESP32 TCP server & write to log file
def get_data ():
  tnow = time.ctime ()
  
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
    print (tnow, ' ', end='')

    # & dump
    print (response, end='')  
	
  except socket.timeout:
    print ("#socket timeout exeption")

  finally:
    s.close ()

while True:  
  # get current time & date
  now = datetime.datetime.now ()

  # check sample interval
  if (now.second%5 == 0):          
    get_data ()

  # relax a bit
  time.sleep (1)













