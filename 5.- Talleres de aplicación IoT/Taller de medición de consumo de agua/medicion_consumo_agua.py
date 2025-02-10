
#!/usr/bin/python
import RPi.GPIO as GPIO
import time, sys
import paho.mqtt.client as mqtt

# Configuración del broker MQTT y el tópico
broker = "localhost"  # Cambia esta dirección si tu broker se encuentra en otra máquina
port = 1883
topic = "codigoIoT/sensor/flujo"

# Configuración del cliente MQTT
client = mqtt.Client()
client.connect(broker, port, 60)

# Configuración del sensor de flujo
pinSensorDeFlujo = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(pinSensorDeFlujo, GPIO.IN, pull_up_down=GPIO.PUD_UP)

global count
count = 0

def countPulse(channel):
    global count
    if start_counter == 1:
        count += 1

GPIO.add_event_detect(pinSensorDeFlujo, GPIO.FALLING, callback=countPulse)

while True:
    try:
        start_counter = 1
        time.sleep(1)
        start_counter = 0
        flow = (count * 60 * 2.25 / 1000)
        # Enviar el dato de flujo a Node-RED vía MQTT
        client.publish(topic, "{:.3f}".format(flow))
        print("El flujo es: %.3f Litros/min" % flow)
        count = 0
        time.sleep(5)
    except KeyboardInterrupt:
        print('\nInterrupción por teclado')
        GPIO.cleanup()
        sys.exit()
        
