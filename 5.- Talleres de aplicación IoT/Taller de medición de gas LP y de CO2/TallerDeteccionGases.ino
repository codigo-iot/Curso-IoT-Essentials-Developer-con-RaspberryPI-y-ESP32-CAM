/*
    ESP32 Devkit V1    Sensor MQ6
    GPIO33------------A0

    ESP32 Devkit V1    Sensor MQ135
    GPIO32------------A0

    LEDs:
    - statusLedPin (GPIO2): Indica el estado de conexi�n WiFi/MQTT.
    - flashLedPin (GPIO4): Indicador de eventos espec�ficos (opcional).
*/

// Bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>

// Datos de red
const char* ssid = "IZZI-D4D2";           // Reemplaza con el nombre de tu red WiFi
const char* password = "COR202D4D2";       // Reemplaza con la contrase�a de tu red WiFi
const char* mqtt_server = "broker.hivemq.com"; // Reemplaza con la direcci�n de tu broker MQTT

// Constante para el t�pico MQTT
const char* MQTT_TOPIC_GASES = "detector_gases/datos"; // T�pico para enviar datos de gases

// Pines de los sensores
#define MQ6_PIN 33      // Pin para conectar el MQ6
#define MQ135_PIN 32    // Pin para conectar el MQ135

// Pines de LEDs
const int statusLedPin = 2;   // LED para indicar conexi�n WiFi/MQTT
const int flashLedPin = 4;    // LED para indicar eventos espec�ficos (opcional)

// Variables para manejo de tiempo
unsigned long lastReconnectAttempt = 0;
const long RECONNECT_WAIT = 5000; // 5 segundos

// Objetos WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Valores de referencia de los sensores en aire limpio (calibrados previamente)
float R0_MQ6 = 9.83;      // Placeholder: debe calibrarse en aire limpio
float R0_MQ135 = 398.63;  // Placeholder: debe calibrarse en aire limpio

// Variables para almacenar ppm
float ppm_mq6 = 0.0;
float ppm_mq135 = 0.0;

// Funci�n para conectarse a WiFi
void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  // Configuraci�n de pines LED
  pinMode(statusLedPin, OUTPUT);
  pinMode(flashLedPin, OUTPUT);
  digitalWrite(statusLedPin, LOW);
  digitalWrite(flashLedPin, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Intentar conectarse a WiFi
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(statusLedPin, HIGH);  // LED encendido mientras intenta conectar
    delay(500);
    digitalWrite(statusLedPin, LOW);   // LED apagado
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Direcci�n IP: ");
  Serial.println(WiFi.localIP());
}

// Funci�n de callback para MQTT (puede dejarse vac�a si no se usa)
void callback(char* topic, byte* payload, unsigned int length) {
  // No se usa en este caso, pero se puede implementar si es necesario
  Serial.print("Mensaje recibido en [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Funci�n para reconectar al broker MQTT
boolean reconnect() {
  Serial.print("Intentando conexi�n MQTT...");
  String clientId = "ESP32Client-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str())) {
    Serial.println("Conectado al broker MQTT");
    // Puedes suscribirte a temas si es necesario
  } else {
    Serial.print("Fallo, rc=");
    Serial.print(client.state());
    Serial.println(" Intentando de nuevo en 5 segundos");
    delay(5000); // Espera antes de reintentar
  }
  return client.connected();
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_WAIT) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }

  // Intervalo para enviar mensajes (cada 2 segundos)
  static unsigned long lastMsg = 0;
  const long interval = 2000; // 2000 ms = 2 segundos
  unsigned long currentMillis = millis();
  if (currentMillis - lastMsg > interval) {
    lastMsg = currentMillis;
    readAndCalculateSensors();
    sendMQTTMessage();
  }
}

void readAndCalculateSensors() {
  // Lectura y c�lculo de MQ6
  int raw_mq6 = analogRead(MQ6_PIN);
  float sensor_volt_mq6 = raw_mq6 / 4095.0 * 3.3; // Conversi�n a voltaje (ADC de 12 bits, 0-3.3V)
  float RS_gas_mq6 = (3.3 - sensor_volt_mq6) / sensor_volt_mq6; // Resistencia del sensor
  float ratio_mq6 = RS_gas_mq6 / R0_MQ6; // Relaci�n RS/R0
  float ppm_log_mq6 = (log10(ratio_mq6) - 2.3) / -1.36; // F�rmula de la hoja de datos para LPG (ajusta seg�n tu gas)
  ppm_mq6 = pow(10, ppm_log_mq6); // Convertir a ppm

  // Lectura y c�lculo de MQ135
  int raw_mq135 = analogRead(MQ135_PIN);
  float sensor_volt_mq135 = raw_mq135 / 4095.0 * 3.3; // Conversi�n a voltaje (ADC de 12 bits, 0-3.3V)
  float RS_gas_mq135 = (3.3 - sensor_volt_mq135) / sensor_volt_mq135; // Resistencia del sensor
  float ratio_mq135 = RS_gas_mq135 / R0_MQ135; // Relaci�n RS/R0
  float ppm_log_mq135 = (log10(ratio_mq135) - 2.3) / -1.36; // F�rmula de la hoja de datos para el aire (ajusta seg�n tu gas)
  ppm_mq135 = pow(10, ppm_log_mq135); // Convertir a ppm

  // Depuraci�n: Imprimir valores en el monitor serial
  Serial.print("MQ6 Voltaje: ");
  Serial.print(sensor_volt_mq6);
  Serial.print(" V, MQ6 ppm: ");
  Serial.print(ppm_mq6);
  Serial.print(" | MQ135 Voltaje: ");
  Serial.print(sensor_volt_mq135);
  Serial.print(" V, MQ135 ppm: ");
  Serial.println(ppm_mq135);
}

void sendMQTTMessage() {
  // Crear cadena JSON manualmente
  char jsonBuffer[100];
  sprintf(jsonBuffer, "{\"MQ6\":%.2f,\"MQ135\":%.2f}", ppm_mq6, ppm_mq135);

  // Depuraci�n: Imprimir el JSON antes de enviarlo
  Serial.print("Publicando en MQTT: ");
  Serial.println(jsonBuffer);

  // Publicar el mensaje JSON en el t�pico MQTT
  if (client.publish(MQTT_TOPIC_GASES, jsonBuffer)) {
    Serial.println("Mensaje MQTT enviado exitosamente:");
    Serial.println(jsonBuffer);
  } else {
    Serial.println("Error al enviar el mensaje MQTT");
  }
}
