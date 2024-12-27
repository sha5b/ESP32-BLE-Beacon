# ESP32 BLE Beacon

A robust implementation of an iBeacon using ESP32 and the NimBLE library. This project turns your ESP32 into a configurable Bluetooth Low Energy (BLE) beacon that follows Apple's iBeacon protocol specifications.

## Features

- 🔷 Full iBeacon protocol compliance
- 📡 Maximum transmission power for better range
- 🔄 Automatic advertising recovery
- 📊 Configurable UUID, Major, and Minor values
- 🔌 WiFi connectivity for remote management
- 🔋 Power-efficient implementation
- 📝 Detailed serial logging

## Hardware Requirements

- ESP32 Development Board (tested on ESP32-WROOM-32D)
- USB cable for programming
- Power source (USB or battery)

## Dependencies

This project uses PlatformIO as the development environment and requires the following dependencies:

- Arduino framework for ESP32
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) (^1.4.1) - Lightweight BLE library

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/ESP32-BLE-Beacon.git
   cd ESP32-BLE-Beacon
   ```

2. Create a `config_secrets.h` file in the `include` directory with your WiFi credentials:
   ```cpp
   #ifndef CONFIG_SECRETS_H
   #define CONFIG_SECRETS_H

   const char* WIFI_SSID = "your_wifi_ssid";
   const char* WIFI_PASSWORD = "your_wifi_password";

   #endif // CONFIG_SECRETS_H
   ```

3. Open the project in PlatformIO and build/upload to your ESP32.

## Configuration

### Basic Configuration
- Device name and update intervals can be modified in `include/config.h`
- WiFi credentials should be set in `include/config_secrets.h`

### Beacon Configuration
The iBeacon parameters can be configured in `src/main.cpp`:

```cpp
bleBeacon.init(
    "8ec76ea3-6668-48da-9866-75be8bc86f4d",  // UUID
    1,                                        // Major
    1,                                        // Minor
    -59                                       // Measured Power
);
```

You can generate your own UUID at [UUID Generator](https://www.uuidgenerator.net/).

## Usage

1. After uploading the code, the ESP32 will:
   - Initialize and connect to WiFi
   - Start broadcasting as an iBeacon
   - Monitor and maintain the advertising state

2. The device will automatically:
   - Use maximum transmission power for optimal range
   - Restart if WiFi connection is lost
   - Monitor and restore advertising if it stops
   - Log status information via Serial (115200 baud)

3. You can modify beacon parameters at runtime using the following methods:
   ```cpp
   bleBeacon.setMajor(newMajorValue);
   bleBeacon.setMinor(newMinorValue);
   bleBeacon.setMeasuredPower(newPowerValue);
   ```

## Serial Monitor

Connect to the serial monitor at 115200 baud to view:
- Device initialization status
- WiFi connection details
- Beacon advertising status
- Error messages and debugging information

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) library for efficient BLE implementation
- [Espressif](https://www.espressif.com/) for the ESP32 platform
