# file    : serport.py
# author  : rb
# purpose : python serial port IO class with threading
# date    : 220801
# last    : 220807

import serial
import time
import threading

# number of items in receive string
LINE_LEN = 51

## serial port  
#class serialPort (threading.Thread):
class serialPort ():
  def __init__ (self, port, baud):
   #threading.Thread.__init__(self)
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
    time.sleep (0.1)

    while True:
      dat = self.port.readline ()
      return (dat.decode ())

err_cnt = 0
  
## MOISTER data parsing
class parseLog:
  def __init__ (self):
    pass

  ## format data from log file - in: string, out: list
  def format (self, str):
    # remove double spaces & '\n' from string (noice, but why???)
    tmp = ' '.join(str.split ())
    
    # split to list
    lst = list(tmp.split(" "))
    
    return (lst)

  ## get MOISTER packet counter
  def count_get (self, lst):
    global err_cnt

    if lst is None:
      return
   
    if (len(lst) != LINE_LEN):
      print (err_cnt, lst)
      err_cnt += 1
      return

    return (int(lst[1]))

  ## get MOISTER board voltages 
  def volt_get (self, lst):
    global err_cnt

    if lst is None:
      return

    if (len(lst) != LINE_LEN):
      print (err_cnt, lst)
      err_cnt += 1
      return

    res = (float(lst[3]), float(lst[5]), float(lst[7]))
    return (res)

  ## get MOISTER temperatures
  def temp_get (self, lst):   
    global err_cnt

    if lst is None:
      return

    if (len(lst) != LINE_LEN):
      print (err_cnt, lst)
      err_cnt += 1
      return

    res = (float(lst[9]), float(lst[10]), float(lst[11]), float(lst[12]))
    return (res)

  ## get MOISTER soil moisture
  def soil_get (self, lst):  
    global err_cnt

    if lst is None:
      return

    if (len(lst) != LINE_LEN):
      print (err_cnt, lst)
      err_cnt += 1
      return

    res = (float(lst[14]), float(lst[15]), float(lst[16]))
    return (res)

  ## get BLE link status
  def link_get (self, lst):  
    global err_cnt

    if lst is None:
      return

    if (len(lst) != LINE_LEN):
      print (err_cnt, lst)
      err_cnt += 1
      return

    res = (int(lst[18]), int(lst[19]), int(lst[20]), int(lst[21]), int(lst[22]), 
           int(lst[23]), int(lst[24]), int(lst[25]), int(lst[26]), int(lst[27]), 
           int(lst[28]), int(lst[29]), int(lst[30]), int(lst[31]), int(lst[32]), 
           int(lst[33]), int(lst[34]), int(lst[35]), int(lst[36]), int(lst[37]), 
           int(lst[38]), int(lst[39]), int(lst[40]), int(lst[41]), int(lst[42]), 
           int(lst[43]), int(lst[44]), int(lst[45]), int(lst[46]), int(lst[47]), 
           int(lst[48]), int(lst[49]), int(lst[50])) 

    return (res)
  
  ## dump MOISTER board voltages 
  def volt_dump (self, lst):
    for v in lst:
      print (v)

  ## dump MOISTER temperatures
  def temp_dump (self, lst):
    for t in lst:
      print (t) 

  ## dump MOISTER soil moisture
  def soil_dump (self, lst):
    for m in lst:
      print (m) 
