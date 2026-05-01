# RV-C-Tool
 
This is a basic tool for sniffing an RV-C bus, decoding status and commands, tracking changes, and sending commands.  It was developed for the SK Pang Electronics ESP32-CAN Bus module distributed by Copper Hill Technologies in the US:  https://copperhilltech.com/esp32-wifi-bluetooth-classic-ble-can-bus-module/

## Features
- RV-C integration for status and control of all devices over the RV-C network, duplicating (and improving) Firefly controls
- Web control panel for Firefly controls and additonal automations
- New automation controls added to Victron screen
- Remote cloud control through VRM
- Gorgeous mini AMOLED remote control panel for bed area remote control
- Timers for inverter, starlink and water pump
- Sleep timer to turn off devices at a set time each night
- Sets the clock on the Firefly display so it's always correct

The purpose of this project is to create a small remote control for the bed area that would allow at-a-glance status views and remote control of systems. I also wanted additional features such as a sleep timer to turn off devices automatically at night, and the option to power up some systems on a short timer (ie. turn on inverter for 1hr). Once setup, the system is very stable and reliable.

The selected microcontroller with capacitive touch AMOLED display is ideal for this purpose and the display can automatically be set to a very low level at night that wont disturb sleep.

I used Ruuvi tags which are small wireless thermometers to get the temperature inside and outside the van. They integrate directly with the Cerbo over Bluetooth.

The Node RED flow can be installed on the Cerbo without using the remote AMOLED display.  This will provide all other functionality and the web dashboard.

## Technology Components
- **Victron Cerbo:** The Cerbo acts as the technology hub and gateway for the project and is enabled with a MQTT sever and Node RED.  A Node RED flow is used as the RV-C to MQTT interface, provides the timer automations, provides the web panel, and provides the additional Cerbo on-screen controls. The MQTT server acts as the gateway to the AMOLED panel.  Both Node RED and the MQTT server are supported by Victron and will survive system updates.
- **Optional Waveshare esp32-s3 micro controller with 1.75" round AMOLED touch display:** This fast and responsive controller has a beautiful and very bright AMOLED display. The firmware is written with ESPHome and then compiled with espressif, so it's fast and reliable. It connects to the Cerbo over wifi using the cerbo built-in access point.  This allows the system to function without any other hardware, yet still allows the cerbo to make a secondary wifi connection for internet if desired. It can be ordered with a case and may be powered with USB-C or through a 5v connection in the back for hard-wiring. The display has a night-time screen saver mode that provides a clock and at-a-glance status that emits very little light so that it isn't an annoyance when trying to sleep.  The display can also turn off at night.
-- Order the version with protective case: https://www.waveshare.com/esp32-s3-touch-amoled-1.75.htm?sku=31262

![Web Dashboard](https://github.com/Erik98146/LaunchControl/blob/main/images/WebDashboard-sm.jpg?raw=true)    ![Victron Screen](https://github.com/Erik98146/LaunchControl/blob/main/images/VictronScreen-sm.jpg?raw=true)     ![AMOLED](https://github.com/Erik98146/LaunchControl/blob/main/images/screen-01.jpg?raw=true)

Video:https://youtu.be/Vuok86__elU
