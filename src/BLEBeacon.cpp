#include "BLEBeacon.h"
#include "config.h"
#include <Arduino.h>

BLEBeacon::BLEBeacon() : major(0), minor(0), measuredPower(-59), isInitialized(false) {
}

void BLEBeacon::init(const char* uuid, uint16_t maj, uint16_t min, int8_t power) {
    // Initialize BLE device with name
    NimBLEDevice::init(DEVICE_NAME);
    
    // Set power level optimized for iOS detection
    // Using medium power level for better iOS compatibility
    NimBLEDevice::setPower(ESP_PWR_LVL_P3);
    
    // Set up scan response data with device name
    NimBLEAdvertisementData scanResponse;
    scanResponse.setName(DEVICE_NAME);
    scanResponse.setFlags(0x06); // BR/EDR Not Supported + LE General Discoverable Mode
    NimBLEDevice::getAdvertising()->setScanResponseData(scanResponse);
    
    // Set iBeacon parameters
    this->uuid = NimBLEUUID(uuid);
    this->major = maj;
    this->minor = min;
    this->measuredPower = power;
    
    isInitialized = true;
    
    Serial.printf("iBeacon initialized with UUID: %s\n", uuid);
}

void BLEBeacon::setupAdvertisementData() {
    // Get the advertising object
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    
    // Create iBeacon advertisement data
    NimBLEAdvertisementData advData;
    
    std::string payload;
    
    // Apple iBeacon prefix with proper flags for iOS
    uint8_t prefix[] = {
        0x02, 0x01, 0x06,        // Flags: LE General Discoverable + BR/EDR Not Supported
        0x1A, 0xFF,              // Manufacturer specific data (26 bytes)
        0x4C, 0x00,              // Apple Company ID (0x004C)
        0x02, 0x15               // iBeacon type and length
    };
    payload.append((char*)prefix, sizeof(prefix));
    
    // UUID
    std::string uuidStr = uuid.toString();
    uint8_t uuidBytes[16];
    for(int i = 0; i < 16; i++) {
        uuidBytes[i] = strtoul(uuidStr.substr(i * 2, 2).c_str(), nullptr, 16);
    }
    payload.append((char*)uuidBytes, 16);
    
    // Major (big endian)
    uint8_t majorBytes[2] = { (uint8_t)(major >> 8), (uint8_t)(major & 0xFF) };
    payload.append((char*)majorBytes, 2);
    
    // Minor (big endian)
    uint8_t minorBytes[2] = { (uint8_t)(minor >> 8), (uint8_t)(minor & 0xFF) };
    payload.append((char*)minorBytes, 2);
    
    // Measured power
    payload.append(1, (char)measuredPower);
    
    // Set the raw advertisement data
    advData.addData(payload);
    pAdvertising->setAdvertisementData(advData);
    
    // Configure advertising parameters optimized for iOS detection
    // iOS works best with 100ms intervals (0x64 = 100ms)
    pAdvertising->setMinInterval(0x64);  // 100ms
    pAdvertising->setMaxInterval(0x64);  // 100ms
    pAdvertising->setMinPreferred(0x64); 
    pAdvertising->setMaxPreferred(0x64);
}

void BLEBeacon::startAdvertising() {
    if (!isInitialized) {
        Serial.println("Error: iBeacon not initialized");
        return;
    }

    setupAdvertisementData();
    
    // Start advertising
    NimBLEDevice::startAdvertising();
    Serial.println("iBeacon started advertising");
}

void BLEBeacon::stopAdvertising() {
    if (!isInitialized) {
        return;
    }
    
    NimBLEDevice::stopAdvertising();
    Serial.println("iBeacon stopped advertising");
}

void BLEBeacon::setMajor(uint16_t maj) {
    major = maj;
    if (isInitialized && NimBLEDevice::getAdvertising()->isAdvertising()) {
        setupAdvertisementData();
    }
}

void BLEBeacon::setMinor(uint16_t min) {
    minor = min;
    if (isInitialized && NimBLEDevice::getAdvertising()->isAdvertising()) {
        setupAdvertisementData();
    }
}

void BLEBeacon::setMeasuredPower(int8_t power) {
    measuredPower = power;
    if (isInitialized && NimBLEDevice::getAdvertising()->isAdvertising()) {
        setupAdvertisementData();
    }
}
