# from: https://github.com/anuranBarman/Python-Chat-Application-Using-PyQt-and-Socket/blob/master/client.py

import sys,time
import socket
from PyQt5 import QtGui
from PyQt5 import QtCore
from PyQt5.QtWidgets import QScrollBar,QSplitter,QTableWidgetItem,QTableWidget,QComboBox,QVBoxLayout,QGridLayout,QDialog,QWidget, QPushButton, QApplication, QMainWindow,QAction,QMessageBox,QLabel,QTextEdit,QProgressBar,QLineEdit
from PyQt5.QtCore import QCoreApplication

from threading import Thread 
#from socketserver import ThreadingMixIn 

c = None

def send_command (cmd):
  host = "192.168.178.47"  # ESP32
  port = 23                # ESP32

  try:
    # instantiate socket & configure socket 
    c = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

    c.settimeout (20)
    c.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	# connect to server  
    c.connect ((host, port))  

    # send 'stat' command
    c.send ('\n'.encode())        # flush possible spurs in ESP32 'recv' buffer
   #c.send ("stat\n".encode())
    c.send (cmd.encode())
  
    # read response
    response = c.recv(1024).decode("UTF-8")  
    print (response, end='')  
	
  except socket.timeout:
    print ("#socket timeout exeption")

  finally:
    c.close ()


class Window (QDialog):
  def __init__(self):
    super().__init__()

    # send 'stat' command
    self.btnStat=QPushButton ("stat",self)
    self.btnStat.resize (50,20)
    self.btnStat.clicked.connect (self.send_stat_cmd)

    # send 'oled' command
    self.btnOled=QPushButton ("OLED",self)
    self.btnOled.resize (50,20)
    self.btnOled.clicked.connect (self.send_oled_cmd)

    self.chatBody=QVBoxLayout (self)

    self.chatBody.addWidget (self.btnStat) # test
    self.chatBody.addWidget (self.btnOled) # test

    self.setWindowTitle ("NFT TCP interface")
    self.resize(100, 50)

  def send_stat_cmd (self):
   #send_command ("stat\n")
    global c
    c.send("\n".encode()) # rb
    c.send("stat\n".encode()) # rb

  def send_oled_cmd (self):
   #send_command ("oled_on\n")
    global c
    c.send("\n".encode()) # rb
    c.send("oled_on\n".encode()) # rb


class ClientThread(Thread):
  def __init__(self,window): 
    Thread.__init__(self) 
    self.window=window
  
  def run (self): 
    global c

    host = "192.168.178.47"  # ESP32
    port = 23                # ESP32
    BUFFER_SIZE = 2000 

    c = socket.socket (socket.AF_INET, socket.SOCK_STREAM, socket.SOL_TCP)

   #c.settimeout (20)
    c.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    c.connect ((host, port))
    
    while True:
      data = c.recv (BUFFER_SIZE)
      print (data.decode("utf-8"))
   
    c.close() 

# main
app = QApplication (sys.argv)

window = Window ()

clientThread = ClientThread (window)
clientThread.start ()

window.exec ()


app.exec ()

