# ESP32 WiFi AP + BLE Beacon Control

This project turns an ESP32 into a configurable WiFi Access Point and BLE Beacon that can be controlled through a web interface.

## Features

- WiFi Access Point with configurable SSID and password
- BLE Beacon with configurable UUID, Major, Minor, and TX Power
- Web interface for easy configuration
- Real-time updates of BLE settings
- Persistent WiFi settings across reboots

## Hardware Requirements

- ESP32 development board
- USB cable for programming
- Computer with web browser for configuration

## Software Requirements

- PlatformIO IDE
- Web browser (Chrome, Firefox, etc.)

## Setup Instructions

1. Clone this repository
2. Open the project in PlatformIO
3. Build and upload the project:
   ```bash
   pio run -t upload
   ```
4. Upload the filesystem (contains web interface):
   ```bash
   pio run -t uploadfs
   ```

## Usage

1. Power on the ESP32
2. Connect to the WiFi network "ESP32_AP" with password "12345678"
3. Open a web browser and navigate to http://192.168.4.1
4. Use the web interface to configure:
   - WiFi AP settings (SSID and password)
   - BLE Beacon settings (UUID, Major, Minor, TX Power)

### WiFi Settings

- Default SSID: ESP32_AP
- Default Password: 12345678
- The device will restart after WiFi settings are changed

### BLE Settings

- Default UUID: 8ec76ea3-6668-48da-9866-75be8bc86f4d
- Default Major: 1
- Default Minor: 1
- Default TX Power: -59

Changes to BLE settings take effect immediately without requiring a restart.

## Development

The project uses:
- Arduino framework for ESP32
- AsyncWebServer for the web interface
- NimBLE for BLE functionality
- SPIFFS for storing web files

## File Structure

- `/src` - Main source code
- `/include` - Header files
- `/data` - Web interface files
- `/lib` - Project libraries

## Troubleshooting

1. If you can't connect to the WiFi AP:
   - Ensure you're using the correct password
   - Try power cycling the ESP32

2. If the web interface doesn't load:
   - Verify you're connected to the ESP32's WiFi network
   - Try accessing http://192.168.4.1 directly

3. If BLE settings don't update:
   - Check the serial monitor for error messages
   - Ensure the UUID format is correct

## License

This project is licensed under the Non-Commercial Open Source License (NCOSL). This means:

- Free for non-commercial use
- Requires attribution
- Source code must remain open source
- Commercial use requires explicit permission

See the LICENSE file for full details.
