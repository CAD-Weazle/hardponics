# file    : sport.py
# author  : rb
# purpose : some python serial port test
# date    : 220722
# last    : 220722

import serial
import time

# <> debug only, remove later
print_types = 0

# define serial port
ser = serial.Serial ()  

# setup serial port
def serial_open (port, baud):
  # set port number & Baudrate
  ser.port = port
  ser.baudrate = int(baud)
  ser.timeout = 1

  # open port
  ser.open ()

  # print port info
  print ("serial port: %s, %d" % (ser.name, ser.baudrate))              

# close serial port
def serial_close ():
  ser.close ()  

# read data from serial port
def sdata_read ():
  sdat = ser.readline ()

  # decode to string
  sstr = sdat.decode ()

  return (sstr)

# check data available
def sdata_waiting ():
  if (ser.inWaiting () > 0):
    return (1)
  else:
    return (0)

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
  #if (print_types):
  #  print (type (ilist[4]))         # class 'str'
  
  print ("humidity   : %2.1f" % (float(ilst[6])))
  #if (print_types):
  #  print (type (ilist[6]))         # class 'str'


# --- main ---

# open serial port
#serial_open ('COM4', 9600)

