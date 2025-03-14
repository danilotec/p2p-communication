#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiClient client;
unsigned long lastSendTime = 0;

void setupWiFi() {
  Serial.println("Conectando ao WiFi do Servidor...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Conectado!");
    Serial.print("Endereço IP do Cliente: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar ao WiFi do Servidor.");
  }
}

void connectToServer() {
  if (!client.connected()) {
    Serial.println("Tentando conectar ao servidor...");
    if (!client.connect(SERVER_IP, SERVER_PORT)) {
      Serial.println("Falha ao conectar ao servidor!");
    } else {
      Serial.println("Conectado ao servidor!");
    }
  }
}

void sendData(int analogValue) {
  connectToServer();

  if (client.connected()) {
    // Criando o JSON
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["device"] = "esp-client";
    jsonDoc["sensor"] = analogValue;
    jsonDoc["timestamp"] = millis();

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    // Enviando JSON
    Serial.println("Enviando JSON:");
    Serial.println(jsonString);
    client.println(jsonString);
  } else {
    Serial.println("Não foi possível enviar, sem conexão com o servidor.");
    delay(2000);
  }
}

void sendCommand(String command) {
  connectToServer();

  if (client.connected()) {
    Serial.println(command);
    client.println(command);
    delay(2000);
  } else {
    Serial.println("Falha ao enviar o comando!");
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ANALOG_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);
  setupWiFi();
}

void loop() {
  if (millis() - lastSendTime > SEND_INTERVAL) {
    int sensorValue = analogRead(ANALOG_PIN);
    sendData(sensorValue);
    lastSendTime = millis();
  }
  if (digitalRead(SWITCH_PIN) == 1) {
    sendCommand("on");
    Serial.println(digitalRead(SWITCH_PIN));
  } else if (digitalRead(SWITCH_PIN) == 0) {
    sendCommand("off");
    Serial.println(digitalRead(SWITCH_PIN));
  }
  
}
