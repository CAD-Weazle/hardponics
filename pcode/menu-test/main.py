# file    : main.py
# author  : rb
# purpose : NFT dashboard
# date    : 230220
# last    : 230320

# imports
import sys
import time
import chart

from PyQt5.QtCore import pyqtSignal, QObject, QCoreApplication

from PyQt5.QtWidgets import QApplication
from tcp_client import TimerThread

# ------- MAIN ----------- 

# define NFT Dashboard GUI
app = QApplication ([])

window = chart.myWindow ()
window.move (20, 20)        # move to absolute screen coordinates
window.show ()

# start TCP client timer thread 
timerThread = TimerThread (window)
timerThread.start ()

print ("bonjour??")  # <> dev only

# start the event loop
app.exec ()

print ("jour bon!!") # <> dev only




