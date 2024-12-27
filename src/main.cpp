#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "BLEBeacon.h"

// Create BLE beacon instance
static BLEBeacon bleBeacon;

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
    
    // Initialize WiFi
    initWiFi();
    
    // Initialize and start BLE beacon with name "11"
    bleBeacon.init("11");
    bleBeacon.startAdvertising();
}

void loop() {
    // Monitor WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Connection Lost!");
        delay(1000);
        ESP.restart(); // Restart device if WiFi is lost
    }
    
    delay(UPDATE_INTERVAL);
}
