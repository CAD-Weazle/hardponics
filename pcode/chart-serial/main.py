# file    : main.py
# author  : rb
# purpose : test code
# date    : 220804
# last    : 220907

import time
from serport import serialPort 
from serport import parseLog
import chart
import serial
import random

ofile = open ("moister.log", "a")

# globals
moister_pcnt = 0                       # packet counter
moister_volt = []                      # board voltages [V]
moister_temp = []                      # temperatures   [oC]
moister_soil = []                      # soil moister   [-]
moister_link = []                      # BLE link status

t = [20.0, 21.0, 22.0, 23.0]

# --- main ---
# open serial port
sp = serialPort ("COM3", 9600)

# instantiate log data parser
pl = parseLog ()

# define main window
win = chart.ChartWindow ()

x = range(0,31)
y = [1]*32

while (True):
  # add blocking element to allow for task switch (needed???)
  time.sleep (0.01)

  # get serial data
  sdat = sp.read_data ()

  # handle serial data
  if (sdat != None):
    # convert serial data to list
    inlist = pl.format (sdat)

    print (inlist)

    # get packet counter
    moister_pcnt = pl.count_get (inlist)
   #win.adjust_counter (moister_pcnt)
    print ("CNT:", moister_pcnt, end= ' ')
      
    # get temperatures
    moister_temp = pl.temp_get (inlist)
    print (moister_temp, end=' ')
 
    # get soil moisture
    moister_soil = pl.soil_get (inlist)
    print (moister_soil, end=' ')
    
    # get board voltages
    moister_volt = pl.volt_get (inlist)
    print (moister_volt, end=' ')

    # get BLE links status
    moister_link = pl.link_get (inlist)
#   print (moister_link, end='\n')

    # update charts
    win.update_temp (moister_temp)
    win.update_soil (moister_soil)
    win.update_volt (moister_volt)

    # update link status bargraph
    win.update_linkstatus (moister_link)
    
    # dump log data
    win.update_logdata (sdat)
##
##    # get time
##    now = time.localtime (time.time())
##    ofile.write ("%4d%02d%02d %02d:%02d:%02d " % (now[0], now[1], now[2], now[3], now[4], now[5]))
##
##    # write to logfile
##    ofile.write (sdat.strip('\n'))
##    ofile.flush ()






