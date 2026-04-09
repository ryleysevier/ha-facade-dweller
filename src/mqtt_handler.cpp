#include "mqtt_handler.h"

MqttHandler *MqttHandler::instance = nullptr;

MqttHandler::MqttHandler()
  : mqttClient(wifiClient), faceCallback(nullptr), needsCallback(nullptr),
    lastStatusMs(0), lastReconnectMs(0) {
  instance = this;
}

void MqttHandler::begin() {
  connectWifi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setBufferSize(512);
  mqttClient.setCallback([](char *topic, byte *payload, unsigned int length) {
    if (instance) instance->onMessage(topic, payload, length);
  });
  connectMqtt();
}

void MqttHandler::connectWifi() {
  Serial.printf("WiFi: connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi: connected, IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi: FAILED — running offline");
  }
}

void MqttHandler::connectMqtt() {
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.printf("MQTT: connecting to %s:%d...", MQTT_SERVER, MQTT_PORT);

  bool connected;
  if (strlen(MQTT_USER) > 0) {
    connected = mqttClient.connect(DEVICE_NAME, MQTT_USER, MQTT_PASSWORD);
  } else {
    connected = mqttClient.connect(DEVICE_NAME);
  }

  if (connected) {
    Serial.println(" connected");
    mqttClient.subscribe(TOPIC_MOOD);
    mqttClient.subscribe(TOPIC_PAD);
    mqttClient.subscribe(TOPIC_FACE);
    mqttClient.subscribe(TOPIC_FEED);
    mqttClient.subscribe(TOPIC_PET);
    mqttClient.subscribe(TOPIC_PLAY);
    Serial.println("MQTT: subscribed to tamagotchi/*");
  } else {
    Serial.printf(" FAILED (rc=%d)\n", mqttClient.state());
  }
}

void MqttHandler::loop() {
  // Reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnectMs > 30000) {
      lastReconnectMs = millis();
      connectWifi();
    }
    return;
  }

  if (!mqttClient.connected()) {
    if (millis() - lastReconnectMs > 5000) {
      lastReconnectMs = millis();
      connectMqtt();
    }
  }

  mqttClient.loop();
}

void MqttHandler::publishStatus(int8_t p, int8_t a, int8_t d, const char *moodName, unsigned long uptimeMs) {
  if (!mqttClient.connected()) return;
  if (millis() - lastStatusMs < STATUS_INTERVAL_MS) return;
  lastStatusMs = millis();

  JsonDocument doc;
  doc["mood"] = moodName;
  doc["p"] = p;
  doc["a"] = a;
  doc["d"] = d;
  doc["uptime"] = uptimeMs / 1000;
  doc["wifi_rssi"] = WiFi.RSSI();
  // Needs will be added by caller if available

  char buf[256];
  serializeJson(doc, buf, sizeof(buf));
  mqttClient.publish(TOPIC_STATUS, buf);
}

// --- Message handling ---

void MqttHandler::onMessage(char *topic, byte *payload, unsigned int length) {
  // Null-terminate payload
  char json[512];
  int len = min((unsigned int)511, length);
  memcpy(json, payload, len);
  json[len] = '\0';

  Serial.printf("MQTT: %s -> %s\n", topic, json);

  // Handle needs topics (no JSON parsing needed)
  if (needsCallback) {
    if (strcmp(topic, TOPIC_FEED) == 0) { needsCallback("feed"); return; }
    if (strcmp(topic, TOPIC_PET) == 0)  { needsCallback("pet"); return; }
    if (strcmp(topic, TOPIC_PLAY) == 0) { needsCallback("play"); return; }
  }

  if (!faceCallback) return;

  FaceCommand cmd = {};
  cmd.moodIndex = -1;
  cmd.hue = -1;
  cmd.icon = ICON_ID_COUNT;
  cmd.pattern = EMOJI_NONE;
  cmd.emojiColor = 0xFFFF;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.printf("MQTT: JSON parse error: %s\n", err.c_str());
    return;
  }

  // --- TOPIC: tamagotchi/mood ---
  if (strcmp(topic, TOPIC_MOOD) == 0) {
    const char *name = doc["name"] | "";
    int idx = findMoodByName(name);
    if (idx >= 0) {
      cmd.hasMoodName = true;
      cmd.moodIndex = idx;
      cmd.hasPAD = true;
      cmd.p = MOODS[idx].pleasure;
      cmd.a = MOODS[idx].arousal;
      cmd.d = MOODS[idx].dominance;
    } else {
      Serial.printf("MQTT: mood '%s' not found\n", name);
      return;
    }
  }

  // --- TOPIC: tamagotchi/pad ---
  else if (strcmp(topic, TOPIC_PAD) == 0) {
    cmd.hasPAD = true;
    cmd.p = doc["p"] | (int8_t)0;
    cmd.a = doc["a"] | (int8_t)0;
    cmd.d = doc["d"] | (int8_t)0;
  }

  // --- TOPIC: tamagotchi/face ---
  else if (strcmp(topic, TOPIC_FACE) == 0) {
    // PAD (required)
    cmd.hasPAD = true;
    cmd.p = doc["p"] | (int8_t)0;
    cmd.a = doc["a"] | (int8_t)0;
    cmd.d = doc["d"] | (int8_t)0;

    // Hue override (optional)
    if (doc.containsKey("hue")) {
      cmd.hasHue = true;
      cmd.hue = doc["hue"] | (int16_t)-1;
    }

    // Emoji (optional)
    if (doc.containsKey("icon") || doc.containsKey("fx")) {
      cmd.hasEmoji = true;
      const char *iconStr = doc["icon"] | "";
      const char *fxStr = doc["fx"] | "";
      const char *colorStr = doc["color"] | "";
      cmd.icon = iconFromString(iconStr);
      cmd.pattern = patternFromString(fxStr);
      if (strlen(colorStr) > 0) cmd.emojiColor = colorFromHex(colorStr);
    }
  }

  faceCallback(cmd);
}

// --- Helpers ---

int MqttHandler::findMoodByName(const char *name) {
  for (int i = 0; i < NUM_MOODS; i++) {
    if (strcasecmp(MOODS[i].name, name) == 0) return i;
  }
  return -1;
}

IconId MqttHandler::iconFromString(const char *str) {
  if (strcasecmp(str, "heart") == 0)    return ICON_ID_HEART;
  if (strcasecmp(str, "star") == 0)     return ICON_ID_STAR;
  if (strcasecmp(str, "note") == 0)     return ICON_ID_NOTE;
  if (strcasecmp(str, "question") == 0) return ICON_ID_QUESTION;
  if (strcasecmp(str, "cloud") == 0)    return ICON_ID_CLOUD;
  if (strcasecmp(str, "drop") == 0)     return ICON_ID_DROP;
  if (strcasecmp(str, "snow") == 0)     return ICON_ID_SNOW;
  if (strcasecmp(str, "warn") == 0)     return ICON_ID_WARN;
  if (strcasecmp(str, "mug") == 0)      return ICON_ID_MUG;
  if (strcasecmp(str, "bell") == 0)     return ICON_ID_BELL;
  if (strcasecmp(str, "box") == 0)      return ICON_ID_BOX;
  if (strcasecmp(str, "bolt") == 0)     return ICON_ID_BOLT;
  if (strcasecmp(str, "zzz") == 0)      return ICON_ID_ZZZ;
  if (strcasecmp(str, "party") == 0)    return ICON_ID_PARTY;
  return ICON_ID_COUNT; // none
}

EmojiPattern MqttHandler::patternFromString(const char *str) {
  if (strcasecmp(str, "pupil_replace") == 0) return EMOJI_PUPIL_REPLACE;
  if (strcasecmp(str, "float_above") == 0)   return EMOJI_FLOAT_ABOVE;
  if (strcasecmp(str, "rain_down") == 0)      return EMOJI_RAIN_DOWN;
  if (strcasecmp(str, "orbit") == 0)          return EMOJI_ORBIT;
  if (strcasecmp(str, "eye_sparkle") == 0)    return EMOJI_EYE_SPARKLE;
  if (strcasecmp(str, "bottom_status") == 0)  return EMOJI_BOTTOM_STATUS;
  if (strcasecmp(str, "bg_fill") == 0)        return EMOJI_BG_FILL;
  if (strcasecmp(str, "side_peek") == 0)      return EMOJI_SIDE_PEEK;
  if (strcasecmp(str, "pulse_center") == 0)   return EMOJI_PULSE_CENTER;
  if (strcasecmp(str, "tear_drop") == 0)      return EMOJI_TEAR_DROP;
  return EMOJI_NONE;
}

uint16_t MqttHandler::colorFromHex(const char *str) {
  return (uint16_t)strtoul(str, NULL, 16);
}
