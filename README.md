# PiTracker
<div id="header" align="center">
  <img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExd3dsdWNmYTJ3MzVwYXBtdTFicmplZDZvbGE2d2VwbjVqdDQyZmdsbyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/mFDWuDppjQJjite6FS/giphy.gif" width="500"/>
</div>

## Overview

This project is an item tracker that utilizes the ESP32 microcontroller to monitor and keep track of items. The Esp32 utilizes the ESP_NOW Wi-Fi protocol to connect to other Esp32 boards so they can localize eachother by signal strenght (RSSI).

# Acknowledgments

Well there are some things i've noticed while making this project. There are many communication tools and all of them has pros and cons, i used WiFi which can track long distances, and it actually have a bit of consistency, but is very unstable, specially when there are lots of glass on the environment. The solution was to implement the Kalman filter and do a bit of mean to reduce latency.

## Features

- **Real-Time Tracking**: Monitor the location and status of items in real-time.
- **ESP32 Integration**: Utilize the power and versatility of the ESP32 microcontroller for wireless tracking.
- **Customizable Alerts**: Set up beep alerts based on specific distances.
- **Auto-Slave finder**: Its based on the EspNow example, that cycles through each slave adress.

## Getting Started
  - **I still need to commit the newer changes i've made, i forgot it, but you can use the older one and implement the kallman filter**
  - Clone this repo.
  - install the required libraries for the code and for the board on the IDE (sometimes you have to specify external links to some libraries on the IDE).
  - Open the project on the arduino IDE.
  - Put the Master code on one Esp32 board and the Slave code on another.
  - Turn them on, if you inspect the pins on the project, you can do things like light up a led on the slave device pressin a button on the master.

### Prerequisites

- ESP32 microcontroller
- [Arduino IDE](https://www.arduino.cc/en/software) or any compatible development environment
- Required libraries
  - <esp_now.h>
  - <WiFi.h>
  - <esp_wifi.h>
  - <Kalman.h>
  - <LiquidCrystal_I2C.h> Or the specific LCD display you're utilizing.
- Hardware
  - Some leds
  - LCD display
  - Buttons
  - Resistors 220Ohm (Led) 1kOhm (Button)
  - Buzzer
  - Jumpers
  - The protoboard

### Installation

1. Clone the repository:

```bash
git clone https://github.com/your-username/esp32-item-tracker.git
