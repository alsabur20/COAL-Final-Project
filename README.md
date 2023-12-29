# Flammable Gas Detection and Alarm System

This project combines an Arduino-based gas sensor system (programmed in AVR assembly) and an ESP32 module for Wi-Fi connectivity, MQTT publishing, and real-time data storage on Google Firebase.

## Components

- **Arduino**: Gas sensor system programmed in AVR assembly.
- **ESP32**: Handles Wi-Fi connectivity, MQTT communication, and Firebase integration.

## Arduino Code (AVR Assembly)

The Arduino code (written in AVR assembly) interfaces with the gas sensor to detect flammable gas levels. It performs the following tasks:
- Configures I/O Pins
- Sets up Analog to Digital Converter (ADC)
- Reads gas sensor values
- Triggers alarms based on gas concentration levels
- Sends data serially

## ESP32 Code (C Language)

The ESP32 code utilizes Wi-Fi connectivity, MQTT protocol, and Firebase integration to achieve the following:
- Connects to a specified Wi-Fi network
- Publishes gas sensor readings to an MQTT broker (`test.mosquitto.org`)
- Stores sensor readings in real-time on Google Firebase's Realtime Database

## Setup

### Arduino (AVR Assembly)
- Load the AVR assembly code onto the Arduino platform.
- Ensure proper connections with the gas sensor.

### ESP32 (C Language)
- Set up Wi-Fi credentials (`ssid` and `password` variables).
- Modify MQTT broker details (`mqttBroker`, `mqttPort`, `mqttClientId`, and `topic` variables) if necessary.
- Include necessary libraries (`WiFi.h`, `PubSubClient.h`, `ESP32Firebase.h`).

## Usage

1. **Arduino**:
   - Ensure proper power supply and gas sensor connection.
   - The system will continuously monitor gas levels and trigger alarms based on predefined thresholds.

2. **ESP32**:
   - Upload the code onto the ESP32 module.
   - Upon successful connection to Wi-Fi, the ESP32 will publish gas sensor readings to the specified MQTT broker and store the data in Google Firebase.

## Team Members

- [Abdul Sabur](https://github.com/alsabur20)
- [Maryam Waseem](https://github.com/maryamw24)
- [Abdul Rehman](https://github.com/AbdulRehman7590)
- [Noor Ul Huda](https://github.com/noorulhuda789)
  
## Contributing

Contributions to improve the functionality, efficiency, or documentation of this project are welcome. Fork the repository, make your changes, and submit a pull request.
