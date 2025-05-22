# ESP32 VSCode Project

This is a basic ESP32 project configured for development in VSCode/Cursor using ESP-IDF.

## Prerequisites

1. Install VSCode or Cursor
2. Install the ESP-IDF extension in VSCode/Cursor
3. Connect your ESP32 board to your computer via USB

## Project Structure

- `src/main.cpp`: Main source code file
- `CMakeLists.txt`: Project build configuration file
- `sdkconfig`: ESP-IDF configuration file

## Getting Started

1. Open this project in VSCode/Cursor
2. Wait for ESP-IDF extension to initialize the project
3. Connect your ESP32 board
4. Click the "Build" button in ESP-IDF extension (or press Ctrl+E B)
5. Click the "Flash" button to upload (or press Ctrl+E F)
6. Open the Serial Monitor (Ctrl+E M) to see the output

## Features

- Basic LED blink example using the built-in LED on GPIO 2
- Serial output at 115200 baud rate
- ESP-IDF configuration for ESP32 development

## Troubleshooting

- If the upload fails, make sure you have selected the correct COM port
- If you don't see the serial output, verify the baud rate is set to 115200
- Make sure you have the correct USB drivers installed for your ESP32 board
- Ensure ESP-IDF extension is properly configured with the correct ESP-IDF version 