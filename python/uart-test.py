import serial
serialport = serial.Serial('/dev/ttyAMA0', baudrate=9600, timeout=3.0)
while True:
    print serialport.readline()
