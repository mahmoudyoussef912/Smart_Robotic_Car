# Smart Robotic Car (ESP32)

<p align="center">
  <img src="https://img.shields.io/badge/Board-ESP32-0A7EA4?style=for-the-badge" alt="ESP32" />
  <img src="https://img.shields.io/badge/Control-Web%20Dashboard-1E8E3E?style=for-the-badge" alt="Web Dashboard" />
  <img src="https://img.shields.io/badge/Connectivity-WiFi-F28C28?style=for-the-badge" alt="WiFi" />
</p>

<p align="center"><strong>Wireless control car project with a clean browser interface.</strong></p>

## Project Idea

This project builds a smart robotic car controlled over Wi-Fi using ESP32.
The ESP32 creates its own local network and hosts a control page, so the car can be driven from any phone or laptop browser.

## System Flow

1. ESP32 powers on and creates Wi-Fi AP.
2. User connects to network: Smart Robotic Car.
3. User opens dashboard at http://192.168.4.1.
4. Dashboard sends movement and accessory commands.
5. ESP32 applies commands to motor driver, LEDs, and buzzer.

## Technology Stack

| Layer | Technology |
|---|---|
| Controller | ESP32 |
| Firmware | Arduino C/C++ |
| Connectivity | Wi-Fi Access Point |
| Control API | HTTP endpoints |
| UI | Embedded HTML, CSS, JavaScript |
| Motor Driver | L298N |

## Quick Start

1. Open Code/Smart_Robotic_Car.ino in Arduino IDE.
2. Select ESP32 board and COM port.
3. Wire hardware and ensure common ground.
4. Upload sketch to ESP32.
5. Connect to Wi-Fi:
      - SSID: Smart Robotic Car
      - Password: 12345678
6. Open http://192.168.4.1 in browser.

## Visual Assets

![System Diagram](Diagram.png)

Demo video: [Video.mp4](Video.mp4)

## Project Structure

```text
Smart_Robotic_Car/
|-- README.md
|-- Diagram.png
|-- Video.mp4
`-- Code/
       `-- Smart_Robotic_Car.ino
```
