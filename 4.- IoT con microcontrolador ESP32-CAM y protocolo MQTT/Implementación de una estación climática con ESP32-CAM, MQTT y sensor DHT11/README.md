# Implementación de una estación climática con ESP32-CAM, MQTT y sensor DHT11  

Este repositorio contiene todos los **códigos necesarios** para la recopilación y transmisión de datos de temperatura y humedad utilizando el **sensor DHT11**, el **protocolo MQTT** y la placa **ESP32CAM**.  

El objetivo de este proyecto es proporcionar un enfoque práctico para operar y recopilar datos del **DHT11**, utilizando la **biblioteca DHT de Adafruit** para interactuar con el sensor y procesar la información obtenida.  

Posteriormente, los datos de **temperatura y humedad** se enviarán a través de **MQTT**, empleando la **biblioteca PubSubClient**. Los mensajes estarán en formato **JSON**, incluirán un identificador único y las mediciones del sensor. Para verificar la transmisión, se puede utilizar **Mosquitto MQTT**, ya sea en un servidor local o remoto.  

### Características del proyecto:  
- **Lectura de temperatura y humedad** con el sensor DHT11. 
- **Envío de datos en formato JSON** utilizando MQTT. 
- **Integración con Mosquitto MQTT** para pruebas y monitoreo. 
- **Compatibilidad con proyectos de estación climática IoT**. 
- **Código listo para ejecución en ESP32CAM con Arduino**. 

Este repositorio es ideal para quienes desean aprender sobre **IoT**, sensores y comunicación en tiempo real.

**Enlace a la lección:** [Implementación de una estación climática con ESP32-CAM, MQTT y sensor DHT11]()
