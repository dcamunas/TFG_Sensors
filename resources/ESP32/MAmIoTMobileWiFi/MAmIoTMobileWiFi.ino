#include "MAmIoTMobileWiFi.h"

// ================================================================
// ===               Wi-Fi connection to STATION                ===
// ================================================================
bool connecToStation() {
  bool connected = false;
  Serial.print("\nConnecting to STATION:" + String(SSID_STATION));
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(SSID_STATION, PASSWORD_STATION);
  delay(5000);
  if (WiFi.status() != WL_CONNECTED) {
    connected = false;
    Serial.println("\nFailed attempt of connection to the STATION:" + String(SSID_STATION));
  } else {
    Serial.println("\n*******WiFi connected to STATION: " + String(SSID_STATION) + "*******");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connected = true;
  }
  return connected;
}

// ================================================================
// ===                     MQTT CONNECTION                      ===
// ================================================================

void reconnect() {
  // Loop until we're reconnected to the MQTT broker
  while (!client.connected()) {
    Serial.println("\nAttempting MQTT connection...");
    if (WiFi.status() != WL_CONNECTED) { //If it is disconnected from the STATION
      Serial.println("\nReconnecting to the WiFi STATION");
      connecToStation();
    } else {
      // Attempt to connect
      const char* DEVICE_ID = "mobile1";
      if (client.connect(DEVICE_ID)) {
        Serial.println("*******Connected to the MQTT broker*******");
        // Subscribe to ...
        //client.subscribe(SUB_TOPIC, QOS_SUBSCRIBE);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}

// ================================================================
// ===                     Global setup                         ===
// ================================================================
void setup() {
  Serial.begin(115200);

  Serial.println("\nMAmIoT Mobile setup...");

  // Connection to STATION
  while (!connecToStation()); // Block till connected to the STATION

  // Setup MQTT broker address and callback
  client.setServer(MQTT_SERVER, MQTT_PORT);

}

void scanAndPublishBeaconData() {

  Serial.println("\nSCAN start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks(false, false, false, 50);
  Serial.println("SCAN done");

  String json = "{\"data\":[";

  if (n == 0) {
    Serial.println("NO networks found");
  } else {
    Serial.print(n);
    Serial.println(" Networks found");

    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");

      // Convert data to JSON string
      String jsonScan_i =
        "{"
        "\"SSID\": \"" + String(WiFi.SSID(i)) + "\","
        "\"RSSI\": \"" + String(WiFi.RSSI(i)) + "\""
        "}";

      json += jsonScan_i;

      if (i != n - 1) {
        json += ",";
      }
      delay(10);
    }
    Serial.println("");
  }
  json += "]}";
  char jsonChar[json.length() + 1];
  json.toCharArray(jsonChar, json.length() + 1);
  Serial.println(jsonChar);
  client.publish(MESSAGE_MOBILE_TOPIC, jsonChar);
}

void loop() {
  if (!client.connected()) {
    reconnect(); // Reconnect to MQTT broker
  } else {
    // Scan beacons and publish:
    scanAndPublishBeaconData();
  }
  delay(50);
  client.loop(); // Requirement to receive message from subscribed topics
}
