import serial
import time
import struct
serialport = serial.Serial('/dev/ttyAMA0', baudrate=115200, timeout=3.0)
i = 0
ctu = True
while True:
    # print bytes(i)
    serialport.write(struct.pack("B",i))
    time.sleep(0.01)
    if ctu and i < 255:
        i += 1
    if i == 255:
        ctu = False
    if not ctu and i > 0:
        i -= 1
    if i == 0 and not ctu:
        ctu = True
