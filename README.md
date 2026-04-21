# Smart Robotic Car (ESP32)

<p align="center">
      <img src="https://img.shields.io/badge/Platform-ESP32-0A7EA4?style=for-the-badge" alt="ESP32" />
      <img src="https://img.shields.io/badge/Control-Web%20Dashboard-1E8E3E?style=for-the-badge" alt="Web Dashboard" />
      <img src="https://img.shields.io/badge/Setup-5%20Minutes-F28C28?style=for-the-badge" alt="Setup 5 Minutes" />
</p>

## Executive Summary

Smart Robotic Car is a Wi-Fi controlled vehicle built on ESP32.
It provides a browser-based control panel for driving and accessory control without requiring a mobile app or external router.

## Business Value

- Fast deployment for demos, labs, and student projects
- Easy operation from any phone or laptop browser
- Clean architecture based on standard ESP32 + Arduino tooling
- Ready-to-present format for technical showcases

## What You Need

- ESP32 development board
- L298N motor driver
- DC motors and car chassis
- LEDs and active buzzer
- Power source for motors and controller

## Start in 5 Minutes

1. Open `Code/Smart_Robotic_Car.ino` in Arduino IDE.
2. Select your ESP32 board and COM port.
3. Wire components according to your circuit diagram and ensure common ground.
4. Upload the sketch.
5. Connect your phone or PC to Wi-Fi:
       - SSID: `Smart Robotic Car`
       - Password: `12345678`
6. Open `http://192.168.4.1` in a browser.

## Expected Result

After connection, the web dashboard should load and allow you to:

- Drive the car in real time
- Change speed levels
- Control lights and horn

## Project Assets

- Wiring diagram: [Diagram.png](Diagram.png)
- Demo video: [Video.mp4](Video.mp4)
- Main firmware: [Code/Smart_Robotic_Car.ino](Code/Smart_Robotic_Car.ino)

## Project Structure

```text
Smart_Robotic_Car/
|-- README.md
|-- Diagram.png
|-- Video.mp4
`-- Code/
            `-- Smart_Robotic_Car.ino
```
