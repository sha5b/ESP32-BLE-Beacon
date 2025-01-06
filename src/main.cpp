#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <DNSServer.h>
#include "config.h"
#include "BLEBeacon.h"

// Create instances
static BLEBeacon bleBeacon;
AsyncWebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

/**
 * Initialize WiFi in AP mode
 */
void initWiFi() {
    // Set WiFi to AP mode
    WiFi.mode(WIFI_AP);
    
    // Start Access Point (open network, no password)
    WiFi.softAP(AP_SSID);
    Serial.println("WiFi AP Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Configure DNS Server
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    Serial.println("DNS Server started");
}

/**
 * Initialize SPIFFS for serving web files
 */
void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
    Serial.println("SPIFFS mounted successfully");
}

/**
 * Setup web server routes and handlers
 */
void setupServer() {
    // Captive Portal Detection
    server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Microsoft NCSI");
    });
    server.on("/redirect", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.on("/gen_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Microsoft NCSI");
    });
    server.on("/msftconnecttest/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Microsoft Connect Test");
    });
    server.on("/kindle-wifi/wifistub.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    server.on("/library/test/success.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    
    // Handle not found URLs
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    
    // Serve static files
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    
    server.begin();
}

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    Serial.println("ESP32 WROOM-32D Initialization");
    
    // Initialize SPIFFS
    initSPIFFS();
    
    // Initialize WiFi AP
    initWiFi();
    
    // Setup web server
    setupServer();
    
    // Initialize and start BLE beacon
    bleBeacon.init(DEFAULT_BLE_UUID, DEFAULT_BLE_MAJOR, DEFAULT_BLE_MINOR, DEFAULT_BLE_TXPOWER);
    bleBeacon.startAdvertising();
    
    Serial.println("Setup Complete");
}

void loop() {
    static unsigned long lastAdvertisingCheck = 0;
    const unsigned long ADVERTISING_CHECK_INTERVAL = 5000;
    
    // Process DNS requests
    dnsServer.processNextRequest();
    
    // Periodically restart advertising to ensure visibility
    unsigned long currentMillis = millis();
    if (currentMillis - lastAdvertisingCheck >= ADVERTISING_CHECK_INTERVAL) {
        lastAdvertisingCheck = currentMillis;
        
        if (!NimBLEDevice::getAdvertising()->isAdvertising()) {
            Serial.println("Advertising stopped - restarting...");
            bleBeacon.startAdvertising();
        }
    }
    
    delay(10); // Reduced delay for better DNS response
}
