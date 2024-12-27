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
    
    // Create advertisement data
    NimBLEAdvertisementData advData;
    
    // Set advertising flags for maximum visibility
    advData.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    
    // Set the complete name
    advData.setName(deviceName);
    
    // Set appearance as generic beacon
    advData.setAppearance(0x0640); // Generic Tag appearance
    
    // Set manufacturer specific data to improve visibility
    uint8_t manData[] = {0x4C, 0x00}; // Generic manufacturer data
    advData.setManufacturerData(std::string((char*)manData, 2));
    
    // Set TX power level to maximum
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Maximum power level
    
    // Set the advertisement data
    pAdvertising->setAdvertisementData(advData);
    
    // Configure advertising parameters for maximum visibility
    pAdvertising->setMinInterval(0x10);  // 10ms minimum interval
    pAdvertising->setMaxInterval(0x20);  // 20ms maximum interval
    pAdvertising->setMinPreferred(0x10);  // Use minimum preferred
    pAdvertising->setMaxPreferred(0x20);  // Use maximum preferred
    
    // Enable active scanning response
    NimBLEAdvertisementData scanResponse;
    scanResponse.setName(deviceName);
    pAdvertising->setScanResponseData(scanResponse);
    
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
