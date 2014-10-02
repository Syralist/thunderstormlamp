import math

maxbrightness = 127.0
period = 4000.0
cycle = 10.0

flashcount = 0
loopcount = 0

while ( flashcount * cycle ) < period:
    brightness = ((math.cos((flashcount*cycle/period)*1000)+math.cos((1.5*flashcount*cycle/period)*1000))+2)/4
    print flashcount, (flashcount*cycle/period)*1000, brightness 
    flashcount += 1
