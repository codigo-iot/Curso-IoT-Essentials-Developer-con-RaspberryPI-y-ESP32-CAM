// Bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Datos de red
const char* ssid = "*****";
const char* password = "******";
const char* mqtt_server = "broker.hivemq.com";

// Datos del sensor
#define DHTPIN 14 // Pin para conectar el DHT11
#define DHTTYPE DHT11 // Modelo de sensor

// Variables para env�o de mensajes
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

// Manejo de LEDs
int statusLedPin = 33;
int flashLedPin = 4;

// Variables para manejo de tiempo
double timeLast, timeNow;
long lastReconnectAttempt = 0;
double WAIT_MSG = 10000;
double RECONNECT_WAIT = 5000;

// Temas MQTT
const char* mqttMsg = "codigoIoT/esp32/msg";
const char* mqttCallback = "codigoIoT/esp32/callback";
const char* mqttDHT = "codigoIoT/mqtt/clima";

// Objetos WiFi, MQTT y sensor
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// Funci�n para conectarse a WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(statusLedPin, HIGH);
    delay(500);
    digitalWrite(statusLedPin, LOW);
    Serial.print(".");
    delay(500);
  }

  randomSeed(micros());
  Serial.println("\nConectado a WiFi");
  Serial.println("Direcci�n IP: ");
  Serial.println(WiFi.localIP());
}

// Funci�n Callback, se ejecuta al recibir un mensaje
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en [");
  Serial.print(topic);
  Serial.print("] ");

  // Construcci�n del mensaje recibido
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  // Mostrar el mensaje concatenado
  Serial.print("Mensaje concatenado en una sola variable: ");
  Serial.println(messageTemp);

  // Enciende/apaga el LED Flash al recibir un mensaje
  if (messageTemp == "true") {
    Serial.println("Led encendido");
    digitalWrite(flashLedPin, HIGH);
  } else if (messageTemp == "false") {
    Serial.println("Led apagado");
    digitalWrite(flashLedPin, LOW);
  }
}

// Funci�n de reconexi�n
boolean reconnect() {
  Serial.print("Intentando conexi�n MQTT...");
  String clientId = "ESP32Client-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str())) {
    client.publish(mqttMsg, "Conectado");
    client.subscribe(mqttCallback);
  } else {
    Serial.print("Error rc=");
    Serial.print(client.state());
    Serial.println(" Intentando de nuevo...");
    delay(1000);
  }
  return client.connected();
}

// Configuraci�n inicial
void setup() {
  Serial.begin(115200);
  pinMode(statusLedPin, OUTPUT);
  pinMode(flashLedPin, OUTPUT);
  digitalWrite(statusLedPin, HIGH);
  digitalWrite(flashLedPin, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin();
  timeLast = millis();
  lastReconnectAttempt = timeLast;
}

// Cuerpo principal del programa
void loop() {
  timeNow = millis();

  if (!client.connected()) {
    if (timeNow - lastReconnectAttempt > RECONNECT_WAIT) {
      if (reconnect()) {
        lastReconnectAttempt = timeNow;
      }
    }
  } else {
    client.loop();
  }

  if (timeNow - timeLast > WAIT_MSG && client.connected()) {
    timeLast = timeNow;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    String json = "{\"temp\":\"" + String(t, 1) + "\",\"hum\":\"" + String(h, 1) + "\"}";
    Serial.println(json);
    int str_len = json.length() + 1;
    char char_array[str_len];
    json.toCharArray(char_array, str_len);
    client.publish(mqttDHT, char_array);
  }
}
