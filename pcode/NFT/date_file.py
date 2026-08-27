# file    : date_file.py
# author  : rb
# purpose : some date formating exercises
# date    : 230303
# last    : 230303
#

import time
import datetime

global f

f = open ("dummy", 'w')

while (1):
  # get current time & date
  now = datetime.datetime.now ()

  # creat new log file every 10 seconds (1 hour later)
  if ((now.second == 0)):
    f.close ()

    fname = ("%4d%02d%02d-%02d%02d%02d.log" % (now.year, now.month, now.day, now.hour, now.minute, now.second))

    f = open (fname, 'w')

    print ("create new file %s" % (fname))

  # write dummy data & flush
  f.write ("ff testen\n")
  f.flush ()

  # relax a bit
  time.sleep (1)