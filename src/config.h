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
#define TOPIC_MOOD    "tamagotchi/mood"
#define TOPIC_PAD     "tamagotchi/pad"
#define TOPIC_FACE    "tamagotchi/face"
#define TOPIC_FEED    "tamagotchi/feed"
#define TOPIC_PET     "tamagotchi/pet"
#define TOPIC_PLAY    "tamagotchi/play"
#define TOPIC_STATUS  "tamagotchi/status"

#define DEVICE_NAME   "tamagotchi-01"
#define STATUS_INTERVAL_MS 30000

#endif // CONFIG_H
