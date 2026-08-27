# from: https://clay-atlas.com/us/blog/2020/01/26/python-english-tutorial-package-socket-build-chatbot/

# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file ‘socket_chatbot.ui’
#
# Created by: PyQt5 UI code generator 5.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_MainWindow(object):
  def setupUi(self, MainWindow):
    MainWindow.setObjectName("MainWindow")
    MainWindow.resize (981, 749)
    self.centralwidget = QtWidgets.QWidget(MainWindow)
    self.centralwidget.setObjectName("centralwidget")

    self.textBrowser = QtWidgets.QTextBrowser(self.centralwidget)
    self.textBrowser.setGeometry(QtCore.QRect(0, 0, 981, 591))

    font = QtGui.QFont()
    font.setFamily("Arial")
    font.setPointSize(12)
    font.setBold(True)
    font.setWeight(75)

    self.textBrowser.setFont(font)
    self.textBrowser.setObjectName("textBrowser")
    self.textEdit = QtWidgets.QTextEdit(self.centralwidget)
    self.textEdit.setGeometry(QtCore.QRect(3, 600, 701, 87))

    font = QtGui.QFont()
    font.setFamily("Arial")
    font.setPointSize(12)
    font.setBold(True)
    font.setWeight(75)

    self.textEdit.setFont(font)
    self.textEdit.setObjectName("textEdit")
    self.pushButton = QtWidgets.QPushButton(self.centralwidget)
    self.pushButton.setGeometry(QtCore.QRect(720, 595, 231, 101))

    font = QtGui.QFont()
    font.setFamily("Arial")
    font.setPointSize(28)
    font.setBold(True)
    font.setWeight(75)

    self.pushButton.setFont(font)
    

