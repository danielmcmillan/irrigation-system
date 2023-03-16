#include <pgmspace.h>

/**
 * Software revision number.
 */
#define NET_HOST_REVISION 1

/**
 * Network connection details.
 */
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

/**
 * MQTT connection details.
 */
const char MQTT_BROKER_ENDPOINT[] = "";
const char MQTT_BROKER_PORT = 8883;
const char MQTT_CLIENT_ID[] = "";
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
