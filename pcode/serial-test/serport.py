# file    : serport.py
# author  : rb
# purpose : python serial port IO class with threading
# date    : 220801
# last    : 220822

import serial
import time
import threading
 
class serialPort:
  def __init__ (self, port, baud):
    self.port = serial.Serial (port, baud)

    if not self.port.isOpen ():
      self.port.open ()
 
  def port_open (self):
    if not self.port.isOpen ():
      self.port.open ()
  
  def port_close (self):
    self.port.close ()
  
  def send_data (self, data):
    num = self.port.write (data)
    return (num)
  
  def read_data (self):
    while True:
      ser_dat = self.port.readline ()
      print (ser_dat.decode (), end='')
  

# --- main ---
sport = serialPort ('COM4', 9600)

threading._start_new_thread (sport.read_data, ())

while True:
  time.sleep (0.1)


   