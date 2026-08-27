from threading import Timer
from threading import Thread 

import time

import pyqtgraph as pg

from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtWidgets
from PyQt5.QtCore import pyqtSignal, pyqtSlot, QObject
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QDialog
from PyQt5.QtWidgets import QVBoxLayout
from PyQt5.QtWidgets import QPushButton


class TimerThread (Thread):    # - org
  def __init__ (self, window):  # why 'window' here? runs also without
    Thread.__init__(self)
    self.setDaemon (True)       # set to Deamon for nice exit

    self.win = window

    self.cnt = 0

  def run (self): 
    while (True):
      print (f"Hello world {self.cnt}")
      self.cnt += 1
      time.sleep (1)

      # send Signal
      if (self.cnt >= 3):
        print ("------ GRAB -------")
        self.cnt = 0
        self.win.sendSig ()



class MainWindow (QMainWindow):
  clickt = pyqtSignal ()

  def __init__(self):
    super (MainWindow, self).__init__()

   #self.clickt.connect (self.addSamples)
   #self.clickt.connect (clickmans)
    self.clickt.connect (self.scrGrab)

    # define 
    self.win = QDialog ()

    # sample data
    hour = [1,2,3,4,5,6,7,8,9,10]
    temp = [30,32,34,32,33,31,29,32,35,45]
   
    # define plot area
    self.pltChart = pg.PlotWidget ()
    
    # define button    
    self.addButton = QPushButton ("Add")
   #self.addButton.clicked.connect (self.sendSig)
   #self.addButton.clicked.connect (clickmans)

    # define layout
    self.layout = QVBoxLayout (self.win)
    self.layout.addWidget (self.pltChart)
    self.layout.addWidget (self.addButton)

    # set layout central
    self.setCentralWidget (self.win)

    # plot sample data
    self.pltChart.plot (hour, temp)

  # emit Signal
  def sendSig (self):
    self.clickt.emit ()

  def scrGrab (self):
    pix = self.win.grab ()
    pix.save ("holla.png")

  # add samples - slot inside MainWindow
  def addSamples (self):
    h = [1,2,3,4,5,6,7,8,9,10]
    t = [130,132,134,132,133,131,129,132,135,145]
   
    self.pltChart.plot (h, t)
   #clickmans ()



# add samples - slot outside MainWindow
def clickmans ():
  h = [1,2,3,4,5,6,7,8,9,10]
  t = [130,132,134,132,133,131,129,132,135,145]

  win.pltChart.plot (h, t)


 #win.add_sample ()


# Parent / Child test
class childFunction (MainWindow):

  def emitSignal (self):
    print ("bonjour!")
    self.sendSig ()


def timertje (p):
  p.emitSignal ()

def hello ():
  print ("hello, world")


# ------------ MAIN ------------
if __name__ == '__main__':
  app = QtWidgets.QApplication ([])

  # define main window
  win = MainWindow ()
  win.show ()

  # emit signal from Parent class
  childInstance = childFunction ()

  # werkt:
 #childInstance.emitSignal ()

  # werkt ook:
 #timertje (childInstance)

 #childInstance.emitSignal ()

  # start timer thread 
  timerThread = TimerThread (win)
  timerThread.start ()

  # add sample to plot <> with signals later
 #win.add_sample ()
 #win.sendSig ()

  app.exec()













