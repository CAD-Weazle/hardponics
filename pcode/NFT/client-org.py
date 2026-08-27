# file    : client.py
# author  : rb
# purpose : TCP client - executed on Windows machine 
# date    : 220201
# last    : 230215
#

import socket  
import time

# ESP32 WIFI module 
host = "192.168.178.47"           # module IP address ESP32 module
port = 23                         # TELNET port number

f = open ("log.dat", 'a')

def get_data ():
  old_reseponse = ""

  # dump timestamp
  print (time.ctime(), ' ', end='')

  f.write (time.ctime()) 
  f.write (' ') 

  try:
    # instantiate socket 
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    # configure socket & connect to server  
    s.settimeout (10)
    s.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.connect ((host, port))  

    # send 'stat' command
    s.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
    s.send ("stat\n".encode())
  
    # read response
    response = s.recv(1024).decode("UTF-8")  

    # & dump
    print (response, end='')  
    f.write (response[0:-1])      # supress empty line in log file

    old_response = response

  except socket.timeout:
    print ("#socket timeout exeption")
    f.write (old_response[0:-1])      # supress empty line in log file


  finally:
    s.close ()
    f.flush ()

while True:  
  # get current time
  now = time.localtime ()

  # check sample interval
# if (now.tm_sec == 0):       # sample each minute 
  if (now.tm_sec%10 == 0):    # sample every 10 seconds
    get_data ()

  # relax a bit
  time.sleep (1)

f.close ()












