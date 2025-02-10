#!/usr/bin/python
import RPi.GPIO as GPIO
import time, sys

pinSensorDeFlujo = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(pinSensorDeFlujo, GPIO.IN, pull_up_down=GPIO.PUD_UP)

global count
count = 0

def countPulse(channel):
    global count
    if start_counter == 1:
        count = count + 1
    flow = count / (60 * 7.5)

GPIO.add_event_detect(pinSensorDeFlujo, GPIO.FALLING, callback=countPulse)

while True:
    try:
        start_counter = 1
        time.sleep(1)
        start_counter = 0
        flow = (count * 60 * 2.25 / 1000)
        print ("El flujo es: %.3f Litros/min" % (flow))
        count = 0
        time.sleep(5)
    except KeyboardInterrupt:
        print ('\nInterrupción por teclado')
        GPIO.cleanup()
        sys.exit() 
