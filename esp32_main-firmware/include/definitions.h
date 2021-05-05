/* Network´s values */
const char *ssid = "Home";
const char *password = "David1208Helena";
const char *mqtt_server_nuc = "192.168.1.141";
const char *mqtt_server_vm = "192.168.1.159";
unsigned int mosquitto_port = 1883;


/* Promiscuous mode */
#define OFFLINE "OFFLINE"
#define ONLINE "ONLINE"
const unsigned int MAX_CHANNEL = 13;    // max Channel -> US = 11, EU = 13, Japan = 14
const unsigned int DEFAULT_TTL = 60;
const unsigned int FIRST_CHANNEL = 1;
const unsigned int LAST_CHANNEL = 13;


/* CO2 configurations */
const unsigned int CO2_PIN = 35;
