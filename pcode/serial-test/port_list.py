import serial
import time
import sys
import glob

def serial_ports ():
  """ Lists serial port names
      returns: A list of the serial ports available on the system
  """
  ports  = ['COM%s' % (i + 1) for i in range(256)]
  result = []
  
  for port in ports:
    try:
      s = serial.Serial (port)
      s.close()
      result.append (port)
    except (OSError, serial.SerialException):
      pass
  return result


if __name__ == '__main__':
  print (serial_ports ())