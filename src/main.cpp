#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

/**
 * Initialize WiFi connection with credentials from config_secrets.h
 * Configuration values are loaded from config.h which includes the secrets
 */
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  Serial.println();
  Serial.print("Connected to WiFi network with IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 WROOM-32D Initialization Complete");
  Serial.printf("Device Name: %s\n", DEVICE_NAME);
  
  initWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected");
  } else {
    Serial.println("WiFi Connection Lost!");
  }
  delay(UPDATE_INTERVAL);
}
