# from: https://clay-atlas.com/us/blog/2020/01/26/python-english-tutorial-package-socket-build-chatbot/

# -*- coding: utf-8 -*-
import sys
import json
import socket
from PyQt5 import QtWidgets
from gui import Ui_MainWindow

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.textBrowser.append('<font color="#FF0000">Server: Hello! I will repeat what you said!</font>')
        self.HOST = "127.0.0.1"
        self.PORT = 23

        self.ui.pushButton.clicked.connect(self.buttonEvent)

    def buttonEvent(self):
        text = self.ui.textEdit.toPlainText()
        self.ui.textEdit.clear()
        self.ui.textBrowser.append('User: '+text)

        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((self.HOST, self.PORT))
        self.client.sendall(text.encode())

        # Server message
        serverMessage = str(self.client.recv(1024), encoding='utf-8')
        self.ui.textBrowser.append('<font color="#FF0000">Server: '+serverMessage+'</font>')
        self.client.close()


if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())