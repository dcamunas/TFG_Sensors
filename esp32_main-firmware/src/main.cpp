#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>
#include <esp_wifi.h>

#include <definitions.h>
#include <iWifi.h>

/* Global variables */
WiFiClient esp_client;
iWifi my_wifi(ssid, password, esp_client);
PubSubClient client(esp_client);

void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type)
{
    String packet, mac_dev;
    unsigned int i, length = 0;
    boolean new_device = true;

    /* Gets payload and its lenght */
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buffer;
    wifi_mgmt_headder *wh = (wifi_mgmt_headder *)pkt->payload;
    length = pkt->rx_ctrl.sig_len - sizeof(wh);

    if (!my_wifi.there_are_dev(length))
    {
        Serial.println("[X] There are not devices in room.");
        return;
    }

    /* 
    This reads the first couple of bytes of the packet. 
    This is where you can read the whole packet replaceing the "8+6+1" (15), 
        with "p->rx_ctrl.sig_len"
    */
    for (i = 8; i < 16; i++)
        packet += String(pkt->payload[i], HEX);

    /*
    This removes the 'nibble' bits from the stat and end of the data we want. 
    So we only get the mac address.
    */
    for (i = 4; i < 16; i++)
        mac_dev += packet[i];
    mac_dev.toUpperCase();

    for (i = 0; i < my_wifi.devices_list.size() && new_device; i++)
        if (mac_dev == my_wifi.devices_list[i].mac)
            new_device = false;

    // If device is new in the room
    if (new_device)
        my_wifi.devices_list.size() == 0 ?
            my_wifi.devices_list.push_back({0, mac_dev, 64, ""}) : my_wifi.devices_list.push_back({int(my_wifi.devices_list.size() - 1), mac_dev, 64, ""});
}  

void start_promiscuous_mode()
{
// Activate promiscuous mode
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);

  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);

  Serial.println("Scanning...");
}



// ------------------------------------------------------------------------------

void setup()
{
  Serial.begin(9600);
  my_wifi.setup_sta_mode();
 // my_wifi.setup_promiscuous_mode();
  //my_wifi.start_promiscuous_mode();

  //client.setServer(mqtt_server, 1883);
}

// ------------------------------------------------------------------------------

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //String clientId = "ESP8266Client";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ------------------------------------------------------------------------------

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();



  
}
