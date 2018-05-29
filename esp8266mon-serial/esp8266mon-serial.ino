// Credits
// Original RTOS version https://github.com/espressif/esp8266-rtos-sample-code/tree/master/03Wifi/Sniffer_DEMO/sniffer
// Converted to Arduino https://github.com/RandDruid/esp8266-deauth and https://github.com/kripthor/WiFiBeaconJam
// Code refactor and improvements Ray Burnette https://www.hackster.io/rayburne/esp8266-mini-sniff-f6b93a
// This version fixes handling of PROBE packets, further refactors code
// This version compiled on Windows 10/Arduino 1.6.5 for Wemos D1 mini but should work on any ESP8266 28Jul2017
// Tested on SDK version:1.5.4(baaeaebb)
// Using Visual Studio Code and the Arduino extension https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino

#include <ESP8266WiFi.h>


#include "functions.h"


#define MAX_APS_TRACKED 50
#define MAX_CLIENTS_TRACKED 100
#undef PRINT_RAW_HEADER   // define this to print raw packet headers
#undef PERIODIC  //define this to get summary of new and expired entries periodically

beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
clientinfo clients_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int clients_known_count = 0;                              // Number of known CLIENTs
probeinfo probes_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int probes_known_count = 0;
#define disable 0
#define enable  1
#define MAX_CLIENT_AGE 1000  //age before entry is considered old (seconds)
#define CHECK_INTERVAL 60   // periodic check interval (seconds)

unsigned int channel = 1;
uint32_t last_check_time, next_check_time;


void setup() {
  Serial.begin(57600);
  Serial.printf("\n\nSDK version:%s\n\r", system_get_sdk_version());
  Serial.println(F("ESP8266 mini-sniff"));
  Serial.println(F("Type:   /-------MAC------/-----WiFi Access Point SSID-----/  /----MAC---/  Chnl  RSSI"));

  wifi_set_opmode(STATION_MODE);            // Promiscuous works only with station mode
  wifi_set_channel(channel);
  wifi_promiscuous_enable(disable);
  wifi_set_promiscuous_rx_cb(promisc_cb);   // Set up promiscuous callback
  wifi_promiscuous_enable(enable);
  last_check_time = 0;
  next_check_time = last_check_time + CHECK_INTERVAL;

}

void loop() {
  channel = 1;
  wifi_set_channel(channel);
  while (true) {
    nothing_new++;                          // Array is not finite, check bounds and adjust if required
    if (nothing_new > 200) {
      nothing_new = 0;
      channel++;
      if (channel == 15) break;             // Only scan channels 1 to 14
      wifi_set_channel(channel);
    }
    delay(1);  // critical processing timeslice for NONOS SDK! No delay(0) yield()
    // Press keyboard ENTER in console with NL active to repaint the screen
    if ((Serial.available() > 0) && (Serial.read() == '\n')) {
      Serial.println("\n-------------------------------------------------------------------------------------\n");
      for (int u = 0; u < clients_known_count; u++) print_client(clients_known[u]);
      for (int u = 0; u < aps_known_count; u++) print_beacon(aps_known[u]);
      for (int u = 0; u < probes_known_count; u++) print_probe(probes_known[u]);
      Serial.println("\n-------------------------------------------------------------------------------------\n");

    }

#ifdef PERIODIC    
    uint32_t now = millis()/1000;
    if (now >= next_check_time) {
      Serial.println("Periodic ");
          
      for (int u = 0; u < clients_known_count; u++) {
        
        if ( !clients_known[u].reported && clients_known[u].last_heard >= last_check_time ) {
          Serial.print("New ");
          print_client(clients_known[u]);
          clients_known[u].reported=1;
          
          
        } else if ( clients_known[u].reported && now > MAX_CLIENT_AGE &&  
                    clients_known[u].last_heard <= (last_check_time - MAX_CLIENT_AGE) ) {
          Serial.print("Old ");
          print_client(clients_known[u]);
          clients_known[u].reported=0;
        }; 
      };
      for (int u = 0; u < aps_known_count; u++) {
        if ( !aps_known[u].reported && aps_known[u].err == 0 
             && aps_known[u].last_heard >= last_check_time ) {
        
          Serial.print("New ");
          print_beacon(aps_known[u]);
          aps_known[u].reported=1;
          
        
           
        } else if ( aps_known[u].reported && aps_known[u].err == 0 && now > MAX_CLIENT_AGE &&  
                    aps_known[u].last_heard <= (last_check_time - MAX_CLIENT_AGE) ) {
          Serial.print("Old ");
          print_beacon(aps_known[u]);
          aps_known[u].reported=0;
        };
      };
      
      
      for (int u = 0; u < probes_known_count; u++) {
      
        if ( !probes_known[u].reported && probes_known[u].last_heard >= last_check_time ) {
           Serial.print("New ");
           print_probe(probes_known[u]);
           probes_known[u].reported=1;
          
        } else if ( probes_known[u].reported && now > MAX_CLIENT_AGE &&  
                    probes_known[u].last_heard <= (last_check_time - MAX_CLIENT_AGE) ) {
          Serial.print("Old ");
          print_probe(probes_known[u]);
          probes_known[u].reported=0;
        }; 
      };
      
      last_check_time = now;
      next_check_time = now + CHECK_INTERVAL;
    };
#endif
  }

}

