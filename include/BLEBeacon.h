#ifndef BLE_BEACON_H
#define BLE_BEACON_H

#include <NimBLEDevice.h>
#include <NimBLEAdvertising.h>
#include <NimBLEUUID.h>

/**
 * @class BLEBeacon
 * @brief Handles iBeacon functionality for ESP32
 */
class BLEBeacon {
public:
    /**
     * @brief Constructor
     */
    BLEBeacon();

    /**
     * @brief Initialize the iBeacon
     * @param uuid The UUID for the iBeacon
     * @param major The major value (default: 0)
     * @param minor The minor value (default: 0)
     * @param measuredPower The measured power at 1m (default: -59)
     */
    void init(const char* uuid, uint16_t major = 0, uint16_t minor = 0, int8_t measuredPower = -59);

    /**
     * @brief Start advertising the iBeacon
     */
    void startAdvertising();

    /**
     * @brief Stop advertising the iBeacon
     */
    void stopAdvertising();

    /**
     * @brief Set the major value
     * @param major The major value to set
     */
    void setMajor(uint16_t major);

    /**
     * @brief Set the minor value
     * @param minor The minor value to set
     */
    void setMinor(uint16_t minor);

    /**
     * @brief Set the measured power
     * @param power The measured power value in dBm
     */
    void setMeasuredPower(int8_t power);

private:
    /**
     * @brief Set up the iBeacon advertisement data
     */
    void setupAdvertisementData();

    NimBLEUUID uuid;
    uint16_t major;
    uint16_t minor;
    int8_t measuredPower;
    bool isInitialized;
};

#endif // BLE_BEACON_H
