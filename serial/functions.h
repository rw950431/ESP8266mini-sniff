// include ESP8266 SDK functions
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#include "structures.h"
struct clientinfo parse_data(uint8_t *frame, uint16_t framelen, signed rssi, unsigned channel);
struct beaconinfo parse_beacon(uint8_t *frame, uint16_t framelen, signed rssi);
struct probeinfo parse_probe(uint8_t *frame, uint16_t framelen, signed rssi);

int register_beacon(beaconinfo beacon);
int register_client(clientinfo ci);
int register_probe(probeinfo pi);

void print_beacon(beaconinfo beacon);
void print_client(clientinfo ci);
void print_probe(probeinfo ci);

void print_pkt_header(uint8_t *buf, uint16_t len, char *pkt_type);

void promisc_cb(uint8_t *buf, uint16_t len);