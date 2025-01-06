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
    
    // Start Access Point with optimal settings for captive portal
    WiFi.softAP(AP_SSID, nullptr, 1, 0, 4); // channel 1, not hidden, max 4 connections
    
    // Disable power-saving mode for better response
    WiFi.setSleep(false);
    Serial.println("WiFi AP Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Configure DNS Server to redirect all domains to our IP
    IPAddress apIP = WiFi.softAPIP();
    // Set DNS to respond with our IP for all domains
    dnsServer.setTTL(0);  // Time-to-live 0 for no caching
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);
    
    // Force DNS settings
    WiFi.softAPsetHostname(AP_SSID);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
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
    // iOS Captive Portal Detection - Return a page that triggers the notification
    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html",
            "<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>Please <a href='http://" +
            WiFi.softAPIP().toString() + "'>click here</a> to continue.</body></html>");
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        request->send(response);
    });

    // iOS Alternate Detection Paths
    server.on("/library/test/success.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html",
            "<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>Please <a href='http://" +
            WiFi.softAPIP().toString() + "'>click here</a> to continue.</body></html>");
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        request->send(response);
    });

    // Android Captive Portal Detection
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString());
    });
    server.on("/gen_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString());
    });

    // Windows/Microsoft Detection
    server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Microsoft NCSI");
    });
    server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Microsoft NCSI");
    });

    // Handle all other URLs
    server.onNotFound([](AsyncWebServerRequest *request) {
        String host = request->host();
        if (host.indexOf("apple") != -1 || host.indexOf("captive") != -1) {
            // iOS devices - return a page that triggers the notification
            AsyncWebServerResponse *response = request->beginResponse(200, "text/html",
                "<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>Please <a href='http://" +
                WiFi.softAPIP().toString() + "'>click here</a> to continue.</body></html>");
            response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
            request->send(response);
        } else {
            // All other requests - redirect to our main page
            request->redirect("http://" + WiFi.softAPIP().toString());
        }
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
    
    // Process DNS requests more frequently for faster response
    delay(1);
}
