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
    // Initialize Serial with proper configuration
    Serial.begin(115200);
    Serial.setRxBufferSize(1024); // Increase buffer size
    Serial.setTimeout(10);        // Lower timeout for faster response
    
    // Wait for serial to be ready
    while (!Serial) {
        delay(10);
    }
    
    delay(1000);
    Serial.println("ESP32 WROOM-32D Initialization Complete");
    
    // Initialize WiFi
    initWiFi();
    
    // Initialize and start BLE beacon with name "11"
    bleBeacon.init("11");
    bleBeacon.startAdvertising();
}

void loop() {
    static unsigned long lastAdvertisingCheck = 0;
    const unsigned long ADVERTISING_CHECK_INTERVAL = 5000; // Check every 5 seconds
    
    // Monitor WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Connection Lost!");
        delay(1000);
        ESP.restart(); // Restart device if WiFi is lost
    }
    
    // Periodically restart advertising to ensure visibility
    unsigned long currentMillis = millis();
    if (currentMillis - lastAdvertisingCheck >= ADVERTISING_CHECK_INTERVAL) {
        lastAdvertisingCheck = currentMillis;
        
        // Check if we're still advertising
        if (!NimBLEDevice::getAdvertising()->isAdvertising()) {
            Serial.println("Advertising stopped - restarting...");
            bleBeacon.startAdvertising();
        }
    }
    
    delay(100); // Short delay to prevent tight loop
}
