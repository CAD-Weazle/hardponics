import os

## globals
packet_cnt = 0               # packet counter as in MOISTER log file
packet_old = 0               # state
packet_err = 0               # dropped packet counter
packet_drp = 0.0             # packet loss [%]

## open log file 
infile = open ("moister2.log", 'r', encoding="utf-8")

## format data from log file - in: string, out: list
def logdata_format (str):
  # remove double spaces & '\n' from string (noice, but why???)
  tmp = ' '.join(str.split ())

  # split to list
  lst = list(tmp.split(" "))
  
  return (lst)

## get packet drop - in: list, out: float
def packetdrop_get (lst):
  global packet_cnt, packet_old, packet_err

  # get packet counter
  packet_cnt = int(lst[1])

  # check for skipped packet
  if ((packet_cnt == packet_old)):
    packet_err += 1
  else:
    packet_old = packet_cnt
 
  # return packet drop 
  try:
    drop_num = (packet_err*100.0)/(packet_cnt)
  except ZeroDivisionError:
    return (0.0)
  else:
    return (drop_num)

## dump packet drop [%]
def packetdrop_dump ():
  global packet_cnt, packet_old, packet_err, packet_drp

  print ("packet drop %d of %d (%2.1f%%)" % (packet_err, packet_cnt, packet_drp))


## --- MAIN ---

# handle MOISTER logdata
for inline in infile:
  # read log file & format
  inlist = logdata_format (inline)

  # get packet drop
  packet_drp = packetdrop_get (inlist)
  packetdrop_dump ()



# close log file
infile.close ()

