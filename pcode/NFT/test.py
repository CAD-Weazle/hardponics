import socket  
import time

# ESP32 WIFI module 
host = "192.168.178.47"
port = 23

def get_data ():
  try:
    # configure socket & connect to server  
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)
    
    # set blocking socket timeout
    s.settimeout (3)
   #to = s.gettimeout ()
   #print ("> time out: %s" % to)
    
    # test this: 
    s.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
   #print ("> connect to ESP")
    s.connect ((host, port))  

   #print ("> send 'stat' command")
    data = "\n";  # only first time, fix this
    s.send (data.encode());
    data = "stat\n";
    s.send (data.encode());
  
   #print ("> get data")
    response = s.recv(1024).decode("UTF-8")  
  
    print (time.ctime(), " ", end="")
    print (response, end="")  

  except socket.timeout:
    print ("#socket timeout exeption")

  finally:
   #print ("> close connection")  
    s.close()

while True:  
  get_data ()

  time.sleep (10)

