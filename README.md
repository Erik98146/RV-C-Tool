# RV-C-Tool
 
This is a basic web based tool for sniffing the RV-C bus, decoding status and commands, tracking changes, and sending commands.  It was developed for the SK Pang Electronics ESP32 CAN-Bus module distributed by Copper Hill Technologies in the US:  

https://copperhilltech.com/esp32-wifi-bluetooth-classic-ble-can-bus-module/

## Features
- Powered by RV-C Bus, 12v, or USB-C
- Decodes the complete specification of RV-C messages
- Finds unknown DGN's
- Decodes individual RV-C messages
- Displays raw data in the same byte format as used in the official [RV-C specification](https://www.rvia.org/media/8401)
- Sorts and filters messages
- Discovers and lists RV-C devices on the bus
- Highlights DGN's with changed/updated data (in yellow)
- Highlights newly discovered DGN's (in orange)
- Hosts a WiFi access point, or can join an existing network using a web based GUI
- Sends commands in the same byte format as the official RV-C specification

## Installing

Download the firmware file above.  
There are several ways to flash the firmware onto the board:

### Option A - Web Tool
1. Connect to the board with USB
2. Use the web based ESP tool to flash the firmware:  https://esptool.spacehuhn.com/
3. Select the port
4. Set an address of ```0x0``` and select the firmware for that address
6. Start

### Option B - Espressif Flash Download Tool
1. Download the espressif Flash Download Tool: https://docs.espressif.com/projects/esp-test-tools/en/latest/esp32/production_stage/tools/flash_download_tool.html
2. Set the chip type to ```ESP32```
3. Set the Work Mode to ```Develop```
4. Set Load Mode to ```UART```
5. Select the firmware file downloaded above
6. Enter Flash Address ```0x0```
7. Select Baud Rate ```460800```
8. Start

### Option C - Command Line:
1. Install Python 3.10 or newer (check the box to add Python to PATH)
2. Install esptool ```pip install esptool```
3. Connect the board with USB-C and find the com port from the device manager
4. Open a command prompt and navigate to the folder with the firmware that you downloaded
5. Flash firware - **Replace COM5 with the correct com port:**  ```python -m esptool --chip esp32 --port COM5 --baud 460800 write_flash 0x0 rvc-tool-firmware.bin```

## Use
On first use, connect to the *rvc-tools* access point. Point your web browser to ```192.168.4.1```  You can then interact directly, or use the network config to join the tool to an access point. Once connected, it will tell you the new IP address you can use to find it on the lan.

## Compiling notes (optional)
This was developed for the ESP-IDF and can be compiled with the ESP-IDF tools or from within VS Code (after ESP-IDF has been properly installed).  **Be sure to setup the ESP-IDF (and/or the tool from within VS Code) for ```websocket server support``` before compiling.**

## Known Issues
Initial WiFi connection may require a couple attempts

## Roadmap
Additional decoding with conversions as needed to proper formats
Add new DGN's
Save/Label unknown DGN's
Label instances

## Screen Shots

### Discover Active DGN's (yellow=changed data. orange=new DGN)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap1.jpg?raw=true)   

### Show detailed DGN data (decoded and raw.  Yellow=changed data)
![Web Dashboard 1](https://github.com/Erik98146/RV-C-Tool/blob/main/images/snap2.jpg?raw=true)  


