#include <ESP8266WiFi.h>
#include "config.h"

WiFiClient client;
unsigned long lastSendTime = 0;

void setupWiFi() {
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) { // Timeout de 10s
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Conectado!");
    Serial.print("Endereço IP do Cliente: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar ao WiFi.");
  }
}

void sendData(int analogValue) {
  if (!client.connected()) {
    Serial.println("Conectando ao servidor...");
    if (!client.connect(SERVER_IP, SERVER_PORT)) {
      Serial.println("Falha ao conectar ao servidor!");
      return;
    }
  }

  Serial.print("Enviando valor: ");
  Serial.println(analogValue);

  client.println(String(analogValue));
}

void setup() {
  Serial.begin(115200);
  pinMode(ANALOG_PIN, INPUT);

  setupWiFi();
}

void loop() {
  if (millis() - lastSendTime > SEND_INTERVAL) {
    int sensorValue = analogRead(ANALOG_PIN);
    sendData(sensorValue);
    lastSendTime = millis();
  }
}
