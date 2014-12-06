import wiringpi

io = wiringpi.GPIO(wiringpi.GPIO.WPI_MODE_SYS)
io.pinMode(17, io.OUTPUT)

io.digitalWrite(17, io.LOW)
