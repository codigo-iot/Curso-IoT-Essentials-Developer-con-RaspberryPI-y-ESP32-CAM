// Bibliotecas
#include <WiFi.h> // Biblioteca WiFi
#include <PubSubClient.h> // Biblioteca MQTT
#include "DHT.h" // Biblioteca para el sensor DHT11

// Datos de red
const char* ssid = "********";
const char* password = "********";
const char* mqtt_server = "192.168.*.*";

// Datos del sensor
#define DHTPIN 14 // Pin para conectar el DHT11, pin GPIO13 del ESP32CAM
#define DHTTYPE DHT11 // Configura el modelo de sensor, compatible también con DHT22

// Variables para envio de mensajes
unsigned long lastMsg = 0; // Contador de tiempo mensajes
#define MSG_BUFFER_SIZE	(50) // Buffer para envío de mensajes
char msg[MSG_BUFFER_SIZE]; // Variable para conversión de mensaje

// Manejo de Leds
int statusLedPin = 33;  // Esta variable controla el led de status
int flashLedPin = 4; // Esta variable controla el led flash
bool statusLed = 0;// Bandera que me dice si el led esta encendido o apagado

// Variables para el manejo de tiempo
double timeLast, timeNow; // Variables para el control de tiempo no bloqueante
long lastReconnectAttempt = 0; // Variable para el conteo de tiempo entre intentos de reconexión
double WAIT_MSG = 1000;  // Espera de 1 segundo entre mensajes
double RECONNECT_WAIT = 5000; // Espera de 5 segundos entre conexiones

// Temas MQTT
const char* mqttMsg = "codigoIoT/esp32/msg";
const char* mqttCallback = "codigoIoT/esp32/callback";
const char* mqttDHT = "codigoIoT/esp32/dht";

// Objetos WiFi, MQTT y sensor
WiFiClient espClient; // Objeto para conexion a WiFi
PubSubClient client(espClient); // Objeto para conexión a MQTT
DHT dht(DHTPIN, DHTTYPE); // Objeto que inicia el sensor

// Función para conectarse a WiFi
void setup_wifi() {
  delay(10);
  
  // Mensajes de intento de conexión
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  // Funciones de conexión
  WiFi.mode(WIFI_STA); // STA inicia el micro controlador como cliente, para accespoint se usa WIFI_AP
  WiFi.begin(ssid, password); // Proporcionar características de red

  // Secuencia que hace parpadear los leds durante el intento de conexión. Logica Inversa
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite (statusLedPin, HIGH); // Apagar LED
    delay(500); // Dado que es de suma importancia esperar a la conexión, debe usarse espera bloqueante
    digitalWrite (statusLedPin, LOW); // Encender LED
    Serial.print(".");  // Indicador de progreso
    delay (5); // Espera asimetrica para dar la sensación de parpadeo al led
  }

  // Esta función mejora los resultados de las funciones aleatorias y toma como semilla el contador de tiempo
  randomSeed(micros());

  // Mensajes de confirmación
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Función Callback, Se ejecuta al recivir un mensaje
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en [");
  Serial.print(topic);
  Serial.print("] ");

  // Esta secuencia imprime y construye el mensaje recibido
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); // Imprime
    messageTemp += (char)payload[i]; // Construye el mensaje en una variable String
  }
  Serial.println();

  // Se comprueba que el mensaje se haya concatenado correctamente
  Serial.println();
  Serial.print ("Mensaje concatenado en una sola variable: ");
  Serial.println (messageTemp);
  
  // Enciende/apaga el LED Flash al recibir un mensaje.
  if(messageTemp == "true") {
    Serial.println("Led encendido");
    digitalWrite(flashLedPin, HIGH);
  }
  else if(messageTemp == "false") {
    Serial.println("Led apagado");
    digitalWrite(flashLedPin, LOW);
  }
}

// Función de recomexión. Devuelve valor booleano para indicar exito o falla
boolean reconnect() {
  Serial.print("Intentando conexión MQTT...");
  // Generar un Client ID aleatorio
  String clientId = "ESP32Client-";
  clientId += String(random(0xffff), HEX);
  // Intentar conexión
  if (client.connect(clientId.c_str())) {
    // Una vez conectado, dar retroalimentación
    client.publish(mqttMsg,"Conectado");
    // Función de suscripción
    client.subscribe(mqttCallback);
  } else {
      // En caso de error
      Serial.print("Error rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo");
      // Esperar 1 segundo
      delay(1000);
    }
  return client.connected();
}

// Inicio del programa
void setup() {

  // Inician las comunicaciones
  Serial.begin(115200);
  // Configuración de pines
  pinMode (statusLedPin, OUTPUT);// Se configura el pin como salida
  pinMode (flashLedPin, OUTPUT);// Se configura el pin como salida
  digitalWrite (statusLedPin, HIGH);// Se comienza con el led apagado
  digitalWrite (flashLedPin, LOW);// Se comienza con el led apagado

  // Iniciar conexiones  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin(); // Función que inicia el sensor

  // Iniciar secuencias de tiempo
  timeLast = millis (); // Inicia el control de tiempo
  lastReconnectAttempt = timeLast; // Control de secuencias de tiempo
}

// Cuerpo del programa
void loop() {

  // Funcion principal de seguimiento de tiempo
  timeNow = millis();

  // Función de conexion y seguimiento
  if (!client.connected()) { // Se pregunta si no existe conexión
    if (timeNow - lastReconnectAttempt > RECONNECT_WAIT) { // Espera de reconexión
      // Intentar reconectar
      if (reconnect()) {
        lastReconnectAttempt = timeNow; // Actualización de seguimiento de tiempo
      }
    }
  } else {
    // Funcion de seguimiento
    client.loop();
  }
  
  // Enviar un mensaje cada WAIT_MSG
  if (timeNow - timeLast > WAIT_MSG && client.connected() == 1) { // Manda un mensaje por MQTT cada cinco segundos
    timeLast = timeNow; // Actualización de seguimiento de tiempo

    // Obtener temperatura y humedad
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Secuencia que se asegura de que la conexión con el sensor exista
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    //Se construye el string correspondiente al JSON que contiene 3 variables
    String json = "{\"id\":\"Usuario\",\"temp\":"+String(t)+",\"hum\":"+String(h)+"}";
    Serial.println(json); // Se imprime en monitor solo para poder visualizar que el string esta correctamente creado
    int str_len = json.length() + 1;//Se calcula la longitud del string
    char char_array[str_len];//Se crea un arreglo de caracteres de dicha longitud
    json.toCharArray(char_array, str_len);//Se convierte el string a char array    
    client.publish(mqttDHT, char_array); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor
  }// fin del if (timeNow - timeLast > wait)
}
