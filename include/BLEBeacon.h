#ifndef BLE_BEACON_H
#define BLE_BEACON_H

#include <NimBLEDevice.h>
#include <NimBLEAdvertising.h>

/**
 * @class BLEBeacon
 * @brief Handles BLE beacon functionality for ESP32
 */
class BLEBeacon {
public:
    /**
     * @brief Constructor
     */
    BLEBeacon();

    /**
     * @brief Initialize the BLE beacon with a given name
     * @param deviceName Name to advertise the beacon as
     */
    void init(const char* deviceName);

    /**
     * @brief Start advertising the BLE beacon
     */
    void startAdvertising();

    /**
     * @brief Stop advertising the BLE beacon
     */
    void stopAdvertising();

private:
    const char* deviceName;
    bool isInitialized;
};

#endif // BLE_BEACON_H
