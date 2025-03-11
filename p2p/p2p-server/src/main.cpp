#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiServer server(1234);
WiFiClient client;
unsigned long lastSendTime = 0;

void setupWiFi() {
  Serial.println("Conectando ao WiFi...");
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) { // Timeout de 10 segundos
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao WiFi!");
    Serial.print("Endereço IP (STA): ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar ao WiFi.");
  }

  WiFi.softAP("ESP_SERVER", "12345678");
  Serial.print("Endereço IP (AP): ");
  Serial.println(WiFi.softAPIP());
}

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconectando ao WiFi...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) { // 10 segundos de tentativa
      delay(500);
      Serial.print(".");
      timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconectado!");
    } else {
      Serial.println("\nFalha ao reconectar ao WiFi.");
    }
  }
}

void sendData(int analogValue) {
  reconnectWiFi();  // Apenas reconecta sem mudar o modo WiFi

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, API_URL);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["sensor"] = analogValue;
    jsonDoc["timestamp"] = millis();

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    int responseCode = http.POST(jsonString);
    
    Serial.print("Código de Resposta: ");
    Serial.println(responseCode);

    if (responseCode > 0) {
      Serial.println("Dados enviados com sucesso!");
    } else {
      Serial.print("Erro ao enviar: ");
      Serial.println(http.errorToString(responseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado! Dados não enviados.");
  }
}

void receiveData() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Novo cliente conectado!");

    unsigned long startTime = millis();
    
    while (client.connected()) {
      if (client.available()) {
        String msg = client.readStringUntil('\n');
        msg.trim(); // Remove espaços em branco e quebras de linha extras

        if (msg.length() > 0) {
          Serial.println("Mensagem recebida de outro ESP: " + msg);
          
          // Aqui você pode processar a mensagem ou enviá-la para a API
          sendData(msg.toInt()); // Se for um valor numérico, enviamos para a API
        }
      }

      // Timeout para evitar travamento (5 segundos)
      if (millis() - startTime > 5000) {
        Serial.println("Timeout! Cliente desconectado.");
        break;
      }
    }

    client.stop();
    Serial.println("Cliente desconectado.");
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ANALOG_PIN, INPUT);

  setupWiFi();
  server.begin();
}

void loop() {
  if (millis() - lastSendTime > SEND_INTERVAL) {
    int sensorValue = analogRead(ANALOG_PIN);

    Serial.print("Valor Analógico: ");
    Serial.println(sensorValue);

    sendData(sensorValue);
    lastSendTime = millis();
  }

  receiveData(); // Recebe mensagens de outros ESPs
}