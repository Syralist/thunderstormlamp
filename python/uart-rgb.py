import serial
import time
import struct
serialport = serial.Serial('/dev/ttyAMA0', baudrate=115200, timeout=3.0)
i = 0
while True:
    print "Rot: "
    i = int(raw_input())
    if i < 0:
        i = 0
    elif i > 254:
        i = 254
    serialport.write(struct.pack("B",i))
    print "Gruen: "
    i = int(raw_input())
    if i < 0:
        i = 0
    elif i > 254:
        i = 254
    serialport.write(struct.pack("B",i))
    print "Blau: "
    i = int(raw_input())
    if i < 0:
        i = 0
    elif i > 254:
        i = 254
    serialport.write(struct.pack("B",i))
    serialport.write(struct.pack("B",255))
