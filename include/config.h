#ifndef CONFIG_H
#define CONFIG_H

// WiFi Station Configuration (connection to your home network)
#define WIFI_SSID "Internet1234"
#define WIFI_PASSWORD "3lvv113LVV11_:)"

// WiFi AP Configuration (ESP32's own network)
#define AP_SSID "GameOfLife"
// No password for open AP

// Device Configuration
#define DEVICE_NAME "GameOfLife"
#define UPDATE_INTERVAL 5000  // milliseconds
#define DEFAULT_BLE_UUID "8ec76ea3-6668-48da-9866-75be8bc86f4d"
#define DEFAULT_BLE_MAJOR 1
#define DEFAULT_BLE_MINOR 1
#define DEFAULT_BLE_TXPOWER -65  // Calibrated power level for better iOS detection

// Maximum connection attempts before restart
#define MAX_WIFI_ATTEMPTS 20

#endif // CONFIG_H
