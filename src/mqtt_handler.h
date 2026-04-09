#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "moods.h"
#include "emoji_fx.h"

// Callback type for when a face change is requested
struct FaceCommand {
  bool hasPAD;
  int8_t p, a, d;
  bool hasHue;
  int16_t hue;         // -1 = auto
  bool hasEmoji;
  IconId icon;
  EmojiPattern pattern;
  uint16_t emojiColor;
  bool hasMoodName;
  int moodIndex;       // -1 if not found
};

typedef void (*FaceCallback)(const FaceCommand &cmd);
typedef void (*NeedsCallback)(const char *action); // "feed", "pet", "play"

class MqttHandler {
public:
  MqttHandler();

  void begin();
  void loop();
  void setFaceCallback(FaceCallback cb) { faceCallback = cb; }
  void setNeedsCallback(NeedsCallback cb) { needsCallback = cb; }

  // Publish current status
  void publishStatus(int8_t p, int8_t a, int8_t d, const char *moodName, unsigned long uptimeMs);

  bool isConnected() { return mqttClient.connected(); }
  bool isWifiConnected() { return WiFi.status() == WL_CONNECTED; }

private:
  WiFiClient wifiClient;
  PubSubClient mqttClient;
  FaceCallback faceCallback;
  NeedsCallback needsCallback;
  unsigned long lastStatusMs;
  unsigned long lastReconnectMs;

  void connectWifi();
  void connectMqtt();
  void onMessage(char *topic, byte *payload, unsigned int length);
  static MqttHandler *instance; // for static callback routing

  int findMoodByName(const char *name);
  IconId iconFromString(const char *str);
  EmojiPattern patternFromString(const char *str);
  uint16_t colorFromHex(const char *str);
};

#endif
