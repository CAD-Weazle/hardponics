# file    : sertest.py
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
serial_open ('COM4', 9600)

# get data & handle
while (1):
  # read serial port
  iline = sdata_read ()
  print (iline)

  # handle data
  ilist = sdata_parse (iline)

  # dump data
  sdata_dump (ilist)

  print ("-------------------------------------------------\n")

# close serial port
serial_close ()









# output line
#ser.write (str.encode ("MOISTER: 2632 VDDA: 2.80 Vsol: 0.75 Vbat: 3.421  NTC: 22.6 23.8 24.1 24.2  SOIL: 2448.0 699.0 558.0\n"))

