# RV-C-Tool
 
This is a basic web based tool for sniffing the RV-C bus, decoding status and commands, tracking changes, and sending commands.  It was developed for the SK Pang Electronics ESP32 CAN-Bus module distributed by Copper Hill Technologies in the US:  

https://copperhilltech.com/esp32-wifi-bluetooth-classic-ble-can-bus-module/

## Features
- Powered by RV-C Bus, 12v, or USB-C
- Decodes the complete specification of RV-C messages
- Decodes individual RV-C messages
- Displays raw data in the same byte format as used in the official [RV-C specification](https://www.rvia.org/media/8401)
- Sorts and filters messages
- Discovers and lists RV-C devices on the bus
- Highlights DGN's with changed/updated data
- Highlights newly discovered DGN's
- Hosts a WiFi access point, or can join an existing network using a web based GUI
- Sends commands in the same byte format as the official RV-C specification

## Compiling
This was developed for the ESP-IDF and can be compiled with the ESP-IDF tools or from within VS Code (after ESP-IDF has been properly installed).  **Be sure to setup the ESP-IDF (and/or the tool from within VS Code) for websocket server support before compiling.**

## Use
On first use, connect to the *rvc-tools* access point. Point your web browser to 192.168.4.1  You can then interact directly, or use the network config to join the tool to an access point. Once connected, it will tell you the new IP address you can use to find it.

## Known Issues
WiFi connectivity is buggy and may require a couple connection attempts

## Roadmap
Additional decoding with conversions as needed to proper formats

## Screen Shots

### Discover Active DGN's (yellow=changed data. orange=new DGN)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap1.jpg?raw=true)   

### Show detailed DGN data (decoded and raw.  Yellow=changed data)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap2.jpg?raw=true)  


