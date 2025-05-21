#!/usr/bin/env python3
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

# Configuración de los LEDs
ledVerde = 21  # GPIO 21
ledRojo = 20  # GPIO 20

# Inicializa los pines GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(ledVerde, GPIO.OUT)
GPIO.output(ledVerde, GPIO.LOW)
GPIO.setup(ledRojo, GPIO.OUT)
GPIO.output(ledRojo, GPIO.LOW)

# Inicializa el lector RFID
reader = SimpleMFRC522()

# Lee el ID almacenado previamente desde el archivo
with open("ID_almacenado.txt", "r") as file:
    id_almacenado = int(file.read().strip())

print("Acerca una tarjeta...")

try:
    while True:
        id, text = reader.read()  # Lee el ID de la tarjeta escaneada
        print(f"ID leído: {id}")

        # Verifica si el ID coincide con el almacenado
        if id == id_almacenado:
            print("Acceso autorizado")
            GPIO.output(ledVerde, GPIO.HIGH)  # Enciende el LED verde
            GPIO.output(ledRojo, GPIO.LOW)    # Apaga el LED rojo
        else:
            print("Acceso denegado")
            GPIO.output(ledRojo, GPIO.HIGH)   # Enciende el LED rojo
            GPIO.output(ledVerde, GPIO.LOW)   # Apaga el LED verde
except KeyboardInterrupt:
    print("Programa interrumpido por el usuario")
finally:
    GPIO.cleanup()  # Limpia los pines GPIO al finalizar
    exit()    
  
