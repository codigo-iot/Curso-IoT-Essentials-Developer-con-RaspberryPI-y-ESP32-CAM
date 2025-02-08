# estacion_climatica.py
# Script para leer datos de temperatura y humedad usando DHT11 y enviar a un broker MQTT en formato JSON.

import time
import adafruit_dht
import paho.mqtt.client as mqtt
from board import D4  # Cambia 'D4' por el pin GPIO que estás utilizando
import json

# Configuración del sensor DHT11
sensor = adafruit_dht.DHT11(D4)

# Configuración del broker MQTT
broker = "broker.hivemq.com"  # Cambia esto por la dirección de tu broker MQTT si es necesario
topic = "codigoIoT/mqtt/clima"

client = mqtt.Client()
client.connect(broker)

# Función para leer datos del sensor con manejo de errores
def leer_datos_sensor():
    try:
        temperatura = sensor.temperature
        humedad = sensor.humidity

        if temperatura is not None and humedad is not None:
            return temperatura, humedad
        else:
            return None, None

    except RuntimeError as e:
        print(f"Advertencia: {e}")
        return None, None

# Bucle principal
try:
    while True:
        # Leer datos del sensor
        temperatura, humedad = leer_datos_sensor()

        # Si los datos son válidos, publícalos en MQTT
        if temperatura is not None and humedad is not None:
            # Crear un mensaje en formato JSON
            mensaje = json.dumps({
                "temp": temperatura,
                "hum": humedad
            })
            print(f"Publicando mensaje: {mensaje}")

            # Publicar en MQTT
            client.publish(topic, mensaje)
        else:
            print("No se pudieron obtener datos del sensor. Intentando nuevamente...")

        # Esperar 10 segundos antes de la próxima lectura
        time.sleep(10)

except KeyboardInterrupt:
    print("Programa interrumpido por el usuario.")

except Exception as e:
    print(f"Error inesperado: {e}")

finally:
    print("Cerrando el programa...")
    sensor.exit()
