//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200
#define HOSTNAME                DEVICE
#define BUFFER_SIZE             1024
#define HEARTBEAT_INTERVAL      300000
#define FS_VERSION_FILE         "/fsversion"

//--------------------------------------------------------------------------------
// RELAY
//--------------------------------------------------------------------------------

#define RELAY_MODE_OFF          0
#define RELAY_MODE_ON           1
#define RELAY_MODE_SAME         2

#define RELAY_SYNC_ANY          0
#define RELAY_SYNC_NONE_OR_ONE  1
#define RELAY_SYNC_ONE          2
#define RELAY_SYNC_SAME         3

#define RELAY_INCHING_NONE      0
#define RELAY_INCHING_OFF       1
#define RELAY_INCHING_ON        2

// Inching time in seconds
#define RELAY_INCHING_TIME      1

// 0 means OFF, 1 ON and 2 whatever was before
#define RELAY_MODE         		RELAY_MODE_OFF

// 0 means ANY, 1 zero or one and 2 one and only one
#define RELAY_SYNC         		RELAY_SYNC_ANY

// 0 means no inching, 1 means normally off, 2 normally on
#define RELAY_INCHING     		RELAY_INCHING_NONE

// -----------------------------------------------------------------------------
// WIFI & WEB
// -----------------------------------------------------------------------------

#define WIFI_RECONNECT_INTERVAL 300000
#define WIFI_MAX_NETWORKS       3
#define ADMIN_PASS              "fibonacci"
#define HTTP_USERNAME           "admin"
#define WS_BUFFER_SIZE          5
#define WS_TIMEOUT              1800000

// -----------------------------------------------------------------------------
// OTA & NOFUSS
// -----------------------------------------------------------------------------

#define OTA_PORT                8266
#define NOFUSS_SERVER           "http://192.168.1.100"
#define NOFUSS_INTERVAL         3600000

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#define MQTT_SERVER             "192.168.1.100"
#define MQTT_PORT               1883
#define MQTT_TOPIC              "/test/switch/{identifier}"
#define MQTT_RETAIN             true
#define MQTT_QOS                0
#define MQTT_KEEPALIVE          30
#define MQTT_RECONNECT_DELAY    10000
#define MQTT_RELAY_TOPIC        "/relay/%d"
#define MQTT_IP_TOPIC           "/ip"
#define MQTT_VERSION_TOPIC      "/version"
#define MQTT_FSVERSION_TOPIC    "/fsversion"
#define MQTT_HEARTBEAT_TOPIC    "/heartbeat"

#define MQTT_CONNECT_EVENT      0
#define MQTT_DISCONNECT_EVENT   1
#define MQTT_MESSAGE_EVENT      2

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#define NTP_SERVER              "pool.ntp.org"
#define NTP_TIME_OFFSET         1
#define NTP_DAY_LIGHT           true
#define NTP_UPDATE_INTERVAL     1800

// -----------------------------------------------------------------------------
// FAUXO
// -----------------------------------------------------------------------------

#define FAUXMO_ENABLED          0
