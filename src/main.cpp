#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Arduino_JSON.h>
#include "config.h"
#include "BLEBeacon.h"

// Create instances
static BLEBeacon bleBeacon;
AsyncWebServer server(80);

// Default settings from config.h
String apName = AP_SSID;
String apPassword = AP_PASSWORD;
String bleUUID = DEFAULT_BLE_UUID;
int bleMajor = DEFAULT_BLE_MAJOR;
int bleMinor = DEFAULT_BLE_MINOR;
int bleTxPower = DEFAULT_BLE_TXPOWER;

/**
 * Initialize WiFi in both Station and AP modes
 */
void initWiFi() {
    // Set WiFi to AP+STA mode
    WiFi.mode(WIFI_AP_STA);
    
    // Start Access Point
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.println("WiFi AP Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
    
    // Set WiFi to use 2.4GHz band
    WiFi.setHostname("ESP32-Beacon");
    
    // Connect to WiFi network
    Serial.print("Connecting to WiFi network: ");
    Serial.println(WIFI_SSID);
    
    // Disconnect from any previous connection
    WiFi.disconnect(true);
    delay(1000);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_ATTEMPTS) {
        delay(1000);
        Serial.print(".");
        attempts++;
        
        if (attempts % 5 == 0) {
            Serial.println();
            Serial.print("Still trying to connect. Status: ");
            switch(WiFi.status()) {
                case WL_NO_SSID_AVAIL:
                    Serial.println("Network not found");
                    break;
                case WL_CONNECT_FAILED:
                    Serial.println("Wrong password");
                    break;
                case WL_IDLE_STATUS:
                    Serial.println("Idle status");
                    break;
                case WL_DISCONNECTED:
                    Serial.println("Disconnected");
                    WiFi.disconnect(true);
                    delay(1000);
                    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
                    break;
                default:
                    Serial.println(WiFi.status());
            }
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi network");
        Serial.print("Station IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi network");
        Serial.println("Still operating in AP mode");
    }
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
    // Serve static files
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    
    // Get current settings
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
        JSONVar settings;
        settings["apName"] = apName;
        settings["uuid"] = bleUUID;
        settings["major"] = bleMajor;
        settings["minor"] = bleMinor;
        settings["txPower"] = bleTxPower;
        
        String response = JSON.stringify(settings);
        request->send(200, "application/json", response);
    });
    
    // Update WiFi settings
    server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            String json = String((char*)data);
            JSONVar settings = JSON.parse(json);
            
            if (JSON.typeof(settings) == "undefined") {
                request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
                return;
            }
            
            apName = (const char*)settings["apName"];
            apPassword = (const char*)settings["apPassword"];
            
            // Validate password length
            if (apPassword.length() < 8) {
                request->send(400, "application/json", 
                    "{\"success\":false,\"message\":\"Password must be at least 8 characters\"}");
                return;
            }
            
            // Schedule a restart to apply new WiFi settings
            request->send(200, "application/json", 
                "{\"success\":true,\"message\":\"WiFi settings updated. Device will restart.\"}");
            delay(1000);
            ESP.restart();
    });
    
    // Update BLE settings
    server.on("/ble", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            String json = String((char*)data);
            JSONVar settings = JSON.parse(json);
            
            if (JSON.typeof(settings) == "undefined") {
                request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
                return;
            }
            
            bleUUID = (const char*)settings["uuid"];
            bleMajor = (int)settings["major"];
            bleMinor = (int)settings["minor"];
            bleTxPower = (int)settings["txPower"];
            
            // Update BLE beacon
            bleBeacon.init(bleUUID.c_str(), bleMajor, bleMinor, bleTxPower);
            bleBeacon.startAdvertising();
            
            request->send(200, "application/json", 
                "{\"success\":true,\"message\":\"BLE settings updated successfully\"}");
    });
    
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
    bleBeacon.init(bleUUID.c_str(), bleMajor, bleMinor, bleTxPower);
    bleBeacon.startAdvertising();
    
    Serial.println("Setup Complete");
}

void loop() {
    static unsigned long lastAdvertisingCheck = 0;
    const unsigned long ADVERTISING_CHECK_INTERVAL = 5000;
    
    // Periodically restart advertising to ensure visibility
    unsigned long currentMillis = millis();
    if (currentMillis - lastAdvertisingCheck >= ADVERTISING_CHECK_INTERVAL) {
        lastAdvertisingCheck = currentMillis;
        
        if (!NimBLEDevice::getAdvertising()->isAdvertising()) {
            Serial.println("Advertising stopped - restarting...");
            bleBeacon.startAdvertising();
        }
    }
    
    delay(100);
}
