import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
ledVerde = 35
ledRojo = 37
#GPIO.setwarnings(False)    # Ignore warning for now
GPIO.setmode(GPIO.BOARD)
GPIO.setup(ledVerde,GPIO.OUT)
GPIO.output(ledVerde, GPIO.LOW)
GPIO.setup(ledRojo,GPIO.OUT)
GPIO.output(ledRojo,GPIO.LOW)

reader = SimpleMFRC522()
print("Coloca una tarjeta sobre el lector")
while True:
    print("Entro While")
    try:
        print("Entro if")
        id, text = reader.read()
        print(id)
        if id==20608395188:
            print("Estas autorizado")
            GPIO.output(ledVerde,GPIO.HIGH)
        else:
            print("Entro else")
            GPIO.output(ledRojo,GPIO.HIGH)
    finally:
        GPIO.cleanup()
GPIO.setwarnings(False) 
