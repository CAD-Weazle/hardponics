# from: https://clay-atlas.com/us/blog/2020/01/26/python-english-tutorial-package-socket-build-chatbot/

import json
import socket

HOST = "127.0.0.1"                # local host
PORT = 23                         # TELNET port number

server = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
server.bind ((HOST, PORT))

server.listen (5)

while True:
  conn, addr = server.accept ()
  clientMessage = str (conn.recv(1024), encoding='utf-8')
  
  print ('Client message is:', clientMessage)
  
  # Repeat
  serverMessage = clientMessage
  conn.sendall (serverMessage.encode ())
  conn.close ()