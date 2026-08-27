# from: https://github.com/anuranBarman/Python-Chat-Application-Using-PyQt-and-Socket/blob/master/client.py

import sys,time
import socket
from PyQt5 import QtGui
from PyQt5 import QtCore
from PyQt5.QtWidgets import QScrollBar,QSplitter,QTableWidgetItem,QTableWidget,QComboBox,QVBoxLayout,QGridLayout,QDialog,QWidget, QPushButton, QApplication, QMainWindow,QAction,QMessageBox,QLabel,QTextEdit,QProgressBar,QLineEdit
from PyQt5.QtCore import QCoreApplication

from threading import Thread 
#from socketserver import ThreadingMixIn 

tcpClientA = None

class Window (QDialog):
    def __init__(self):
        super().__init__()
        self.flag=0

       #self.chatTextField=QLineEdit(self)
       #self.chatTextField.resize(880,100)
       #self.chatTextField.move(10,350)

        self.btnSend=QPushButton("Send",self)
        self.btnSend.resize(880,30)
        self.btnSendFont=self.btnSend.font()
        self.btnSendFont.setPointSize(15)
        self.btnSend.setFont(self.btnSendFont)
        self.btnSend.move(10,460)
        self.btnSend.setStyleSheet("background-color: #F7CE16")
        self.btnSend.clicked.connect(self.send)

        self.chatBody=QVBoxLayout(self)
      # self.chatBody.addWidget(self.chatTextField)
      # self.chatBody.addWidget(self.btnSend)
      # self.chatWidget.setLayout(self.chatBody)
        splitter=QSplitter(QtCore.Qt.Vertical)

        self.chat = QTextEdit()
        self.chat.setReadOnly(True)

        splitter.addWidget(self.chat)
       #splitter.addWidget(self.chatTextField)
        splitter.setSizes([400,100])

        splitter2=QSplitter(QtCore.Qt.Vertical)
        splitter2.addWidget(splitter)
        splitter2.addWidget(self.btnSend)
        splitter2.setSizes([200,10])

        self.chatBody.addWidget(splitter2)

        self.setWindowTitle("Chat Application")
        self.resize(700, 500)

    def send (self):
       #text=self.chatTextField.text()
       #font=self.chat.font()
       #font.setPointSize(9)
       #self.chat.setFont(font)
       #textFormatted='{:>80}'.format(text)
       #self.chat.append(textFormatted)
       #tcpClientA.send(text.encode())
        tcpClientA.send("stat\n".encode()) # rb
       #self.chatTextField.setText("")

class ClientThread(Thread):
  def __init__(self,window): 
    Thread.__init__(self) 
    self.window=window
  
  def run(self): 
    global tcpClientA

   #host = "127.0.0.1"
   #port = 80
    host = "192.168.178.47"  # ESP32
    port = 23                # ESP32
    BUFFER_SIZE = 2000 

    tcpClientA = socket.socket (socket.AF_INET, socket.SOCK_STREAM) 
    tcpClientA.connect ((host, port))
    
    while True:
      data = tcpClientA.recv (BUFFER_SIZE)
      print (data.decode("utf-8"))
      window.chat.append (data.decode("utf-8"))
   
    tcpClientA.close() 

# main
if __name__ == '__main__':
  app = QApplication (sys.argv)
  window = Window ()

  clientThread=ClientThread (window)
  clientThread.start ()

  window.exec ()
  sys.exit (app.exec_())

