#!/usr/bin/env python3
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
# Inicializa el lector RFID
reader = SimpleMFRC522()
print("Coloca una tarjeta sobre el lector...")

try:
    id, text = reader.read() # Lee el ID y el texto asociado
    print(f"El ID de la tarjeta es: {id}")

    # Guarda el ID en un archivo de texto para uso futuro
    with open("ID_almacenado.txt", "w") as file:
        file.write(str(id))
        print("ID almacenado correctamente.")
finally:
    GPIO.cleanup() # Limpia los pines GPIO al finalizar
  
