# ESP8266mini-sniff
Wifi packet sniffer based on the ESP8266 chipset

The serial version simply reports details of surrounding Wifi devices to the serial port.

Credits:

Original RTOS version https://github.com/espressif/esp8266-rtos-sample-code/tree/master/03Wifi/Sniffer_DEMO/sniffer

Adapted to Arduino https://github.com/RandDruid/esp8266-deauth , https://github.com/kripthor/WiFiBeaconJam and https://git.schneefux.xyz/schneefux/jimmiejammer

Code refactor and improvements https://www.hackster.io/rayburne/esp8266-mini-sniff-f6b93a. This version is purely a sniffer that doesnt interfere with Wifi commuication

This version fixes handling of PROBE packets, further refactors code
This version compiled on Windows 10/Arduino 1.6.5 for Wemos D1 mini but should work on any ESP8266 dev board.
Tested with Expressif SDK version:1.5.4(baaeaebb)
