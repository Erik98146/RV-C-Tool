# RV-C-Tool
 
This is a basic web based tool for sniffing an RV-C bus, decoding status and commands, tracking changes, and sending commands.  It was developed for the SK Pang Electronics ESP32-CAN Bus module distributed by Copper Hill Technologies in the US:  

https://copperhilltech.com/esp32-wifi-bluetooth-classic-ble-can-bus-module/

## Features
- Powered by RV-C Bus, 12v, or USB-C
- Decodes the full specification of RV-C protocol
- Displays raw data in the same byte format as used in the official RV-C specification
- Hosts a WiFi access point, or can join an existing network for a web based GUI
- Discovers and lists RV-C devices on the bus
- Decodes RV-C messages
- Sort and filter messages
- Highlights DGN's with changed/updated data
- Highlights newly discovered DGN's
- Sends commands in the same byte format as the official RV-C specification

## Compiling
This was developed for the ESP-IDF and can be compiled with the ESP-IDF tools or from within VS Code (after ESP-IDF has been properly installed).  **Be sure to setup the ESP-IDF (and/or the tool from within VS Code) for websocket server support before compiling.**

## Use
On first use, connect to the *sniffer* access point.

## Known Issues
WiFi connectivity is buggy and may require a couple connection attempts

## Roadmap
Additional decoding with conversions as needed to proper formats

## Screen Shots

### Discover Active DGN's (yellow=changed data. orange=new DGN)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap1.jpg?raw=true)   

### Show detailed DGN data (decoded and raw.  Yellow=changed data)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap2.jpg?raw=true)  


