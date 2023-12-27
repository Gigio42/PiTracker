# PiTracker
<div id="header" align="center">
  <img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExd3dsdWNmYTJ3MzVwYXBtdTFicmplZDZvbGE2d2VwbjVqdDQyZmdsbyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/mFDWuDppjQJjite6FS/giphy.gif" width="500"/>
</div>

## Overview

This project is an item tracker that utilizes the ESP32 microcontroller to monitor and keep track of items. The Esp32 utilizes the ESP_NOW Wi-Fi protocol to connect to other Esp32 boards so they can localize eachother by signal strenght (RSSI).

## Features

- **Real-Time Tracking**: Monitor the location and status of items in real-time.
- **ESP32 Integration**: Utilize the power and versatility of the ESP32 microcontroller for efficient tracking.
- **User Interface**: Access a user-friendly interface to view and manage tracked items.
- **Customizable Alerts**: Set up alerts and notifications based on specific criteria.

## Getting Started
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
  - <LiquidCrystal_I2C.h> Or the specific LCD display you're utilizing.

### Installation

1. Clone the repository:

```bash
git clone https://github.com/your-username/esp32-item-tracker.git
