#ifndef CONFIG_H
#define CONFIG_H

#define WIFI_SSID "wifi-local"
#define WIFI_PASSWORD "senha_wifi"
#define API_URL "http://192.168.0.11:5000/dados"

#define ANALOG_PIN A0
#define SEND_INTERVAL (0.1 * 60 * 1000)

void setupWiFi();
void sendData(int analogValue);

#endif