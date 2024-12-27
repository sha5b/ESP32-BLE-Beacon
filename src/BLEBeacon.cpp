#include "BLEBeacon.h"
#include <Arduino.h>

BLEBeacon::BLEBeacon() : deviceName(nullptr), isInitialized(false) {
}

void BLEBeacon::init(const char* name) {
    deviceName = name;
    
    // Initialize BLE device
    NimBLEDevice::init(deviceName);
    isInitialized = true;
    
    Serial.printf("BLE Beacon initialized with name: %s\n", deviceName);
}

void BLEBeacon::startAdvertising() {
    if (!isInitialized) {
        Serial.println("Error: BLE Beacon not initialized");
        return;
    }

    // Get the advertising object
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    
    // Clear any existing advertising config
    pAdvertising->reset();
    
    // Set the device name in advertising data
    pAdvertising->setName(deviceName);
    
    // Configure advertising parameters
    pAdvertising->setMinInterval(0x20); // 20ms minimum interval
    pAdvertising->setMaxInterval(0x40); // 40ms maximum interval
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // iPhone connection issue workaround
    pAdvertising->setMaxPreferred(0x12);

    // Start advertising
    NimBLEDevice::startAdvertising();
    Serial.println("BLE Beacon started advertising");
}

void BLEBeacon::stopAdvertising() {
    if (!isInitialized) {
        return;
    }
    
    NimBLEDevice::stopAdvertising();
    Serial.println("BLE Beacon stopped advertising");
}
