const int ledPin = LED_BUILTIN; // Utiliza el LED integrado

unsigned long timeNow = 0;
unsigned long timeLast = 0;
const unsigned long interval = 1000; // Intervalo de parpadeo en milisegundos

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  timeNow = millis();
  if (timeNow - timeLast >= interval) {
    timeLast = timeNow;
    digitalWrite(ledPin, !digitalRead(ledPin)); // Cambia el estado del LED
  }
}
