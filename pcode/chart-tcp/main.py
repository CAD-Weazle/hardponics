# file    : main.py
# author  : rb
# purpose : NFT dashboard - TCP connection
# date    : 230220
# last    : 230220

import time
from serport import serialPort 
from serport import parseLog
import chart
import serial
import socket
import random

# globals
nft_pcnt = 0                      # packet counter                        [-]
nft_vbat = 0                      # battery pack voltage                  [V]
nft_ibat = 0                      # battery pack charge/discharge current [mA]
nft_vsol = 0                      # solar panel voltage                   [V]
nft_temp = 0                      # temperature                           [oC]
nft_humi = 0                      # humidity                              [%RH]
nft_par  = 0                      # sun light intensity                   [PAR]
nft_ec   = 0                      # EC nutrient muxture                   [mS]

# read data from lof file <> dev only
f = open ("log.dat", "r")

# define main window
win = chart.ChartWindow ()

while True:
  line = f.readline()

  # remove double spaces & '\n' from string (noice, but why???)
  tmp = ' '.join(line.split ())

  # split to list
  lst = list (tmp.split(" "))

  if (not line):
    break
  print (line.strip())

  try:
    nft_pcnt = lst[6]
    nft_vbat = lst[8]
    nft_ibat = lst[10]
    nft_vsol = lst[12]
    nft_temp = lst[14]
    nft_humi = lst[16]
    nft_par  = lst[18]
    nft_ec   = lst[22]
  except:
    print ("some input error")

 #print ("packet: %d"  % int(nft_pcnt)  , end=' ')
 #print ("Vbat: %2.2f" % float(nft_vbat), end=' ')
 #print ("Ibat: %2.2f" % float(nft_ibat), end=' ')
 #print ("Vsol: %2.2f" % float(nft_vsol), end=' ')
 #print ("T: %2.2f"    % float(nft_temp), end=' ')
 #print ("RH: %2.2f"   % float(nft_humi), end=' ')
 #print ("PAR: %3.2f"  % float(nft_par) , end=' ')
 #print ("EC: %1.2f"   % float(nft_ec))

  # update chart
  win.update_temp (nft_temp)
  win.update_humi (nft_humi)
  win.update_volt (nft_vbat, nft_vsol, nft_ibat)

  time.sleep (0.1)







#x = range(0,31)
#y = [1]*32
#
#while (True):
#  # add blocking element to allow for task switch (needed???)
#  time.sleep (0.01)
#
#  # get serial data
#  sdat = sp.read_data ()
#
#  # handle serial data
#  if (sdat != None):
#    # convert serial data to list
#    inlist = pl.format (sdat)
#
#    print (inlist)
#
#    # get packet counter
#    moister_pcnt = pl.count_get (inlist)
#   #win.adjust_counter (moister_pcnt)
#    print ("CNT:", moister_pcnt, end= ' ')
#      
#    # get temperatures
#    moister_temp = pl.temp_get (inlist)
#    print (moister_temp, end=' ')
# 
#    # get soil moisture
#    moister_soil = pl.soil_get (inlist)
#    print (moister_soil, end=' ')
#    
#    # get board voltages
#    moister_volt = pl.volt_get (inlist)
#    print (moister_volt, end=' ')
#
#    # get BLE links status
#    moister_link = pl.link_get (inlist)
##   print (moister_link, end='\n')
#
#    # update charts
#    win.update_temp (moister_temp)
#    win.update_soil (moister_soil)
#    win.update_volt (moister_volt)
#
#    # update link status bargraph
#    win.update_linkstatus (moister_link)
#    
#    # dump log data
#    win.update_logdata (sdat)
###
###    # get time
###    now = time.localtime (time.time())
###    ofile.write ("%4d%02d%02d %02d:%02d:%02d " % (now[0], now[1], now[2], now[3], now[4], now[5]))
###
###    # write to logfile
###    ofile.write (sdat.strip('\n'))
###    ofile.flush ()
#
#
#
#
#
#
#