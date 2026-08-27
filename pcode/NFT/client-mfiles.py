# file    : client.py
# author  : rb
# purpose : TCP client - executed on Windows machine 
# date    : 220201
# last    : 230303
#

import socket  
import datetime

# ESP32 WIFI module 
host = "192.168.178.47"           # module IP address ESP32 module
port = 23                         # TELNET port number

f = open ("log.dat", 'a')

def get_data ():
  old_reseponse = ""

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

while True:  
  # get current time & date
  now = datetime.datetime.now ()

  # creat new log file every 10 minutes (1 hour later)
  if (now.minute%10 == 0):
    f.close ()

    fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.year, now.month, now.day, now.hour, now.minute, now.second))

    f = open (fname, 'w')

    print ("create new file %s" % (fname))

  # check sample interval
  if (now.tm_sec == 0):           # sample each minute 
    get_data ()

  # relax a bit
  time.sleep (1)

f.close ()












