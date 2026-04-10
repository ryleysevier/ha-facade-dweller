#ifndef CONFIG_H
#define CONFIG_H

// All credentials come from environment variables via build_flags in platformio.ini.
// Set them in your .env file (see .env.example).
// Fallback defaults are provided for safety.

#ifndef WIFI_SSID
#define WIFI_SSID     "UNCONFIGURED"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER   "localhost"
#endif

#ifndef MQTT_PORT
#define MQTT_PORT     1883
#endif

#ifndef MQTT_USER
#define MQTT_USER     ""
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD ""
#endif

// MQTT topics
#define TOPIC_MOOD    "facade/mood"
#define TOPIC_PAD     "facade/pad"
#define TOPIC_FACE    "facade/face"
#define TOPIC_FEED    "facade/feed"
#define TOPIC_PET     "facade/pet"
#define TOPIC_PLAY    "facade/play"
#define TOPIC_STATUS  "facade/status"

#define DEVICE_NAME   "dweller-01"
#define STATUS_INTERVAL_MS 30000

#endif // CONFIG_H
