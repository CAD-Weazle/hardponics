import socket
import time

# ESP32 WIFI module 
target_host = "192.168.178.47"
target_port = 23

# create a socket connection
client = socket.socket (socket.AF_INET, socket.SOCK_STREAM)

# let the client connect
client.connect ((target_host, target_port))

while (1):
  # get data
  response = client.recv (200)

  print (time.ctime(), " ", end="")
  print (response.decode(), end="")

  # delay 
  time.sleep (1)
  
# bail
exit ()


