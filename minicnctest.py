"""
Javier Martinez Garcia
NOV 2015
GPL 

Python part of the code running here:
https://www.youtube.com/watch?v=VHL-ju65lEk
"""

import serial
import time
ser = serial.Serial( '/dev/ttyACM0', 115200 )
time.sleep(1)
ser.write( 'INIT' )
ser.readline()
# rombo
ser.write( 'POWER ON' )
ser.readline()
ser.write('MOVE 0 40 80 0' )
ser.readline()
ser.write('SPIN 100' )
for i in range(4):
  ser.write( 'MOVE 40 40 0 0' )
  #time.sleep(100)
  ser.readline()
  ser.write( 'MOVE 40 -40 0 0' )
  #time.sleep(100)
  ser.readline()
  ser.write( 'MOVE -40 -40 0' )
  ser.readline()
  ser.write( 'MOVE -40 40 0 0' )
  ser.readline()
  ser.write( 'MOVE 0 0 -20 0' )
  ser.readline()
  ser.write( 'FEED -700 -700' )
  ser.readline()

ser.write( 'SPIN 0' )
ser.readline()
ser.write( 'FEED 3200 3200')
ser.readline()
ser.write('MOVE 0 -40 0 0')
ser.readline()

ser.write( 'POWER OFF')
ser.readline()


