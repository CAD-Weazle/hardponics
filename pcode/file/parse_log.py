# file    : parse_log.py
# author  : rb
# purpose : parse MOISTER log file
# date    : 220810
# last    : 220810

import os

## globals
moister_volt = []                      # board voltages [V]
moister_temp = []                      # temperatures   [oC]
moister_soil = []                      # soil moister   [-]

## open log file 
infile = open ("moister2.log", 'r', encoding="utf-8")

## format data from log file - in: string, out: list
def logdata_format (str):
  # remove double spaces & '\n' from string (noice, but why???)
  tmp = ' '.join(str.split ())

  # split to list
  lst = list(tmp.split(" "))
  
  return (lst)

## get MOISTER board voltages 
def voltages_get (lst):
  global moister_volt
  
  v = (float(lst[3]), float(lst[5]), float(lst[7]))
  moister_volt.append (v) 

## dump MOISTER board voltages 
def voltages_dump (lst):
  for v in lst:
    print (v)

## get MOISTER temperatures
def temperatures_get (lst):
  global moister_volt
  
  t = (float(lst[9]), float(lst[10]), float(lst[11]), float(lst[12]))
  moister_temp.append (t) 

## dump MOISTER temperatures
def temperatures_dump (lst):
  for t in lst:
    print (t)

## get MOISTER soil moisture
def moisture_get (lst):
  global moister_soil
  
  s = (float(lst[14]), float(lst[15]), float(lst[16]))
  moister_soil.append (s) 

## dump MOISTER soil moisture
def moisture_dump (lst):
  for m in lst:
    print (m)

## parse MOISTER log file
def parse_log (lst):
  voltages_get (lst)
  temperatures_get (lst)
  moisture_get (lst)

## dump parsed dsta
def dump_log ():
  global moister_volt
  global moister_temp
  global moister_soil

  for i in range (len(moister_volt)):
    print ("V:", moister_volt[i], end=' ')
    print ("T:", moister_temp[i], end=' ')
    print ("M:", moister_soil[i], end='\n')

 ## --- MAIN ---

# handle MOISTER logdata
for inline in infile:
  # read log file & format
  inlist = logdata_format (inline)
  parse_log (inlist)

dump_log ()
  
# close log file
infile.close ()

