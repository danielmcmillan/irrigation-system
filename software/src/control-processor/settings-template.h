#include <pgmspace.h>

/**
 * Network connection details.
 */
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "password"

/**
 * MQTT connection details.
 */
// #define MQTT_CLIENT_ID "icu-1" // max length 20
#define MQTT_CLIENT_ID "" // max length 20
const char MQTT_BROKER_ENDPOINT[] = "";
const int MQTT_BROKER_PORT = 8883;
static const char MQTT_BROKER_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
static const char MQTT_CLIENT_CERT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";
static const char MQTT_CLIENT_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";
