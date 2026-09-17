# ESP32 Fire Radar

An ESP32 based fire detection radar system using a flame sensor, servo motor, buzzer and red LED.

The system scans an area of 180 degrees using a servo motor. When a flame is detected, the servo stops at the detected angle and the system activates the buzzer and LED.

A live radar interface is displayed on a smartphone through the ESP32 Wi-Fi web server.

## Features

- 180 degree radar scanning
- ESP32 based system
- Flame detection
- Servo motor scanning
- Fire angle detection
- Red LED warning
- Buzzer alarm
- Live radar on smartphone
- ESP32 Wi-Fi access point
- Servo locks on detected fire

## Components

- ESP32
- Servo motor
- Flame sensor
- Active buzzer module
- Red LED
- 220 ohm resistor
- Jumper wires

## Pin Connections

| Component | ESP32 Pin |
|---|---|
| Flame Sensor DO | GPIO 2 |
| Buzzer | GPIO 0 |
| Servo Signal | GPIO 4 |
| Red LED | GPIO 5 |

## How It Works

The servo continuously scans from 0 to 180 degrees.

When the flame sensor detects fire:

1. The servo stops at the detected angle.
2. The red LED turns ON.
3. The buzzer turns ON.
4. The mobile radar shows a red target at the detected angle.
5. When the flame disappears, scanning starts again.

## Mobile Radar

Connect your phone to the Wi-Fi network created by the ESP32.

Wi-Fi Name:

Fire-Radar

Password:

12345678

Open the following address in your phone browser:

192.168.4.1

## Arduino Libraries

- WiFi
- WebServer
- ESP32Servo

## Project

ESP32 Fire Detection Radar with Mobile Web Interface.
