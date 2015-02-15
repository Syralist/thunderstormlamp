import serial
import time
import struct
serialport = serial.Serial('/dev/ttyAMA0', baudrate=115200, timeout=3.0)
r = 254
g = 0
b = 0
while True:
    for g in range(0, 255, 1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

    for r in range(254, 0, -1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

    for b in range(0, 255, 1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

    for g in range(254, 0, -1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

    for r in range(0, 255, 1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

    for b in range(254, 0, -1):
        serialport.write(struct.pack("B",r))
        serialport.write(struct.pack("B",g))
        serialport.write(struct.pack("B",b))
        serialport.write(struct.pack("B",255))
        time.sleep(0.05)

