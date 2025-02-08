#!/usr/bin/env python

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

reader = SimpleMFRC522()

print ("Coloca una tarjeta sobre el lector")
try:
    id,text = reader.read()
    print("El ID de la tarjeta es:\n")
    print(id)
finally:
        GPIO.cleanup()
