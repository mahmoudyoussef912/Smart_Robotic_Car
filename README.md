# Smart Robotic Car (ESP32)

## What is this project?

This project is a smart robotic car controlled over Wi-Fi using an ESP32.
The ESP32 runs as an Access Point and hosts a web page that works as a remote controller.
From the browser, the user can drive the car and control accessories such as lights and horn.

## How does it work?

1. When powered on, the ESP32 creates a Wi-Fi network:
      - SSID: Smart Robotic Car
      - Password: 12345678
2. The ESP32 starts a local web server on:
      - http://192.168.4.1
3. The user connects to this Wi-Fi from phone or laptop and opens the web page.
4. The page sends control commands to the ESP32 (move, stop, speed, lights, horn).
5. The ESP32 processes each command and drives motors/accessories through GPIO pins and motor driver.

## How to run it

1. Open Code/Smart_Robotic_Car.ino in Arduino IDE.
2. Select ESP32 board and COM port.
3. Connect the hardware and ensure common ground.
4. Upload the sketch.
5. Connect to Smart Robotic Car Wi-Fi.
6. Open http://192.168.4.1.

## Project structure

Smart_Robotic_Car/
|- README.md
|- Diagram.png
|- Video.mp4
`- Code/
      `- Smart_Robotic_Car.ino
