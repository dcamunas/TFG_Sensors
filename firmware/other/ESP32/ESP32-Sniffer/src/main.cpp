#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_wifi.h>
//#include <WebServer.h>
//#include <WiFiAP.h>

#define MAX_CHANNEL 13 //max Channel -> US = 11, EU = 13, Japan = 14
#define MAX_DEVICES 64
#define MAC_INDEX 0
#define TTL_INDEX 1
#define STATE_INDEX 2
#define DEFAULT_TTL "60"
#define OFFLINE "OFFLINE"
#define ONLINE "ONLINE"

/* Local Ip definition */
/*IPAddress ip(192,168,2,1);
IPAddress gateway(192,168,2,255);
IPAddress mask(255,255,255,0);*/

/* Promiscuous Filter's mask */
const wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};

/* Mac address */
typedef struct
{
  uint8_t mac[6];
} __attribute__((packed)) mac_address;

/* Packet header's data */
typedef struct
{
  int16_t fctl;
  int16_t duration;
  mac_address da;
  mac_address sa;
  mac_address bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) wifi_mgmt_headder;

unsigned int devices_number = 0, device_id = 0, current_channel = 1; // First channel by default
//String default_TTL = "60";                                // Max value of ttl, elapsed this time, the device is considered offline
String devices_list[MAX_DEVICES][3];

/* Checking of there are device in the room */
boolean there_are_dev(int length)
{
  return !(length < 0);
}

/* Organize packets after get sniffed */
void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type)
{
  String mac, packet;
  int length;
  /* Gets payload and its lenght */
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buffer;
  wifi_mgmt_headder *wh = (wifi_mgmt_headder *)pkt->payload;
  length = pkt->rx_ctrl.sig_len - sizeof(wh);

  if (!there_are_dev(length))
  {
    Serial.println("There are not devices in room.");
    return;
  }

  //int fctl = ntohs(wh->fctl);
  boolean new_device = true;

  /* 
This reads the first couple of bytes of the packet. 
This is where you can read the whole packet replaceing the "8+6+1", 
with "p->rx_ctrl.sig_len"
*/
  for (unsigned int i = 8; i <= 15; i++)
  {
    packet += String(pkt->payload[i], HEX);
  }

  /*
This removes the 'nibble' bits from the stat and end of the data we want. 
So we only get the mac address.
*/
  for (unsigned int i = 4; i <= 15; i++)
  {

    mac += packet[i]; 
  }
  mac.toUpperCase();

  for (unsigned int i = 0; i < MAX_DEVICES && new_device; i++)
  {
    if (mac == devices_list[i][MAC_INDEX])
    {
      // Upgrade TTL
      devices_list[i][TTL_INDEX] = String(DEFAULT_TTL);

      if (devices_list[i][STATE_INDEX] == OFFLINE)
      {
        devices_list[i][STATE_INDEX] = ONLINE;
      }
      new_device = false;
    }
  }

  // If device is new in the room
  if (new_device)
  {
    devices_list[device_id][MAC_INDEX] = mac;
    devices_list[device_id][TTL_INDEX] = DEFAULT_TTL;
    device_id++;
    devices_number++;

    // If devices number is greater than 64, back to 0 index of devices_list
    device_id == 63 ? device_id = 0 : device_id = device_id;
  }
}

void setup()
{
  Serial.begin(9600);
  //delay(10);

  // Wifi setup
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);

  esp_wifi_start();

  // Activate promiscuous mode
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);

  esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);

  Serial.println("Scanning...");
}

void check_max_channel()
{
  if (current_channel > MAX_CHANNEL)
    current_channel = 1;
}

void update_ttl()
{
  int ttl;
  for (unsigned int i = 0; i < MAX_DEVICES; i++)
  {
    ttl = devices_list[i][MAC_INDEX].toInt();
    ttl--;

    ttl <= 0 ? devices_list[i][STATE_INDEX] = OFFLINE : devices_list[i][TTL_INDEX] = String(ttl);
  }
}

void show_people()
{
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    if (!(devices_list[i][MAC_INDEX] == ""))
    {
      Serial.println("[X] DEVICE ID: " + String(i) + " || MAC ADR: " + devices_list[i][MAC_INDEX] + " || TTL: " + devices_list[i][TTL_INDEX] + " || STATE: " + devices_list[i][STATE_INDEX]);
    }
  }
  Serial.println("------------------------------------------------\n \t[X] Totals Devices: "+ String(devices_number));
  Serial.println("------------------------------------------------------------------------------------------------\n\n");
}

void loop()
{
  check_max_channel();
  delay(1000);
  update_ttl();
  show_people();
  esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);

  current_channel++;
}

