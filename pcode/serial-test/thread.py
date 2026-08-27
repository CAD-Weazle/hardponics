# file    : thread.py
# author  : rb
# purpose : some python serial port test
# date    : 220722
# last    : 220723

import serial
import threading
import time

# globals
sstr = ""
snew = 0
scnt = 0

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

# open serial port
ser = serial.Serial ('COM4', 9600)  

# define Thread - use 'daeamon' so Thread can be stopped
#threading._start_new_thread (sdata_read, ())      # old syntax
threading.Thread (target=sdata_read, daemon=True).start ()

# main loop
while True:
  time.sleep (0.1)

  if (snew):
    print (">sample %d" % scnt)
    sdata_dump (sdata_parse (sstr))

    scnt += 1
    snew = 0

# get data & handle
###while (1):
###  # read serial port
###  iline = sdata_read ()
###  print (iline)
###
###  # handle data
###  ilist = sdata_parse (iline)
###
###  # dump data
###  sdata_dump (ilist)
###
###  print ("-------------------------------------------------\n")
###
#### close serial port
###serial_close ()


# output line
#ser.write (str.encode ("MOISTER: 2632 VDDA: 2.80 Vsol: 0.75 Vbat: 3.421  NTC: 22.6 23.8 24.1 24.2  SOIL: 2448.0 699.0 558.0\n"))

