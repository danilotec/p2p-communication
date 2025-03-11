#ifndef CONFIG_H
#define CONFIG_H

#define WIFI_SSID "CLARO_2G9FE495"
#define WIFI_PASSWORD "32525291_Gui"
#define API_URL "http://192.168.0.11:5000/dados"

#define ANALOG_PIN A0
#define SEND_INTERVAL (0.1 * 60 * 1000)

void setupWiFi();
void sendData(int analogValue);

#endif