#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <ArduinoOTA.h>
#include "moods.h"
#include "eye_renderer.h"
#include "mqtt_handler.h"
#include "needs.h"

// XIAO ESP32-C6 -> GC9A01
#define TFT_DC    0
#define TFT_CS    1
#define TFT_RST   2

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 *canvas = NULL;
uint16_t *framebuffer = NULL;
EyeRenderer *eye = NULL;
MqttHandler mqtt;
NeedsEngine needs;

// Current mood tracking
int currentMoodIndex = 0;
const char *currentMoodName = "neutral";
int lastNeedsMoodIdx = -1;

// Handle face commands from MQTT
void onFaceCommand(const FaceCommand &cmd) {
  needs.onMqttCommand(); // any command counts as interaction

  if (cmd.hasMoodName && cmd.moodIndex >= 0) {
    // Preset mood
    currentMoodIndex = cmd.moodIndex;
    currentMoodName = MOODS[cmd.moodIndex].name;
    eye->setMood(MOODS[cmd.moodIndex], cmd.moodIndex);
    Serial.printf("MQTT: mood preset -> %s\n", currentMoodName);
  }
  else if (cmd.hasPAD) {
    float p = cmd.p / 100.0f, a = cmd.a / 100.0f, d = cmd.d / 100.0f;
    currentMoodName = "mqtt_custom";
    currentMoodIndex = -1;

    if (cmd.hasEmoji) {
      eye->setFace(p, a, d,
                    cmd.hasHue ? cmd.hue : -1,
                    cmd.pattern, cmd.icon, cmd.emojiColor);
    } else {
      eye->setFace(p, a, d, cmd.hasHue ? cmd.hue : -1);
    }
    Serial.printf("MQTT: face -> P:%d A:%d D:%d\n", cmd.p, cmd.a, cmd.d);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Display + framebuffer
  canvas = new GFXcanvas16(240, 240);
  if (!canvas || !canvas->getBuffer()) {
    Serial.println("CANVAS ALLOC FAILED!");
    while (1) delay(1000);
  }
  framebuffer = canvas->getBuffer();
  memset(framebuffer, 0, 240 * 240 * sizeof(uint16_t));
  Serial.printf("Canvas OK. Free heap: %d\n", ESP.getFreeHeap());

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(0x0000);

  eye = new EyeRenderer(framebuffer, 240, 240);

  // Try to set timezone for time-of-day awareness
  configTzTime("America/Denver", "pool.ntp.org");

  eye->setMood(MOODS[0], 0);
  currentMoodName = MOODS[0].name;

  // Show connecting indicator
  eye->update();
  canvas->setTextSize(1);
  canvas->setTextColor(0x7BEF);
  canvas->setCursor(75, 220);
  canvas->print("connecting...");
  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);

  // Connect WiFi + MQTT
  mqtt.setFaceCallback(onFaceCommand);
  mqtt.setNeedsCallback([](const char *action) {
    if (strcmp(action, "feed") == 0) needs.feed();
    else if (strcmp(action, "pet") == 0) needs.pet();
    else if (strcmp(action, "play") == 0) needs.entertain();
    Serial.printf("Needs: %s! H:%d E:%d B:%d Joy:%d\n",
      action, (int)needs.getHunger(), (int)needs.getEnergy(),
      (int)needs.getBoredom(), (int)needs.getHappiness());
  });
  mqtt.begin();

  // OTA updates
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.onStart([]() {
    // Show update indicator on screen
    canvas->fillScreen(0x0000);
    canvas->setTextSize(2);
    canvas->setTextColor(0x07E0);
    canvas->setCursor(60, 110);
    canvas->print("Updating...");
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int pct = progress * 100 / total;
    canvas->fillRect(40, 140, 160, 10, 0x0000);
    canvas->fillRect(40, 140, pct * 160 / 100, 10, 0x07E0);
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.onEnd([]() {
    canvas->fillScreen(0x0000);
    canvas->setCursor(70, 110);
    canvas->setTextColor(0x07E0);
    canvas->print("Rebooting");
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.begin();

  Serial.printf("Ready. OTA hostname: %s. Free heap: %d\n", DEVICE_NAME, ESP.getFreeHeap());
}

void loop() {
  ArduinoOTA.handle();
  mqtt.loop();
  needs.update(millis());

  // When idle (no MQTT commands for 60s), let needs engine drive the face
  if (needs.shouldControl()) {
    int needsMoodIdx = needs.getCurrentMoodIndex();
    if (needsMoodIdx != lastNeedsMoodIdx) {
      lastNeedsMoodIdx = needsMoodIdx;
      currentMoodIndex = needsMoodIdx;
      currentMoodName = MOODS[needsMoodIdx].name;
      eye->setMood(MOODS[needsMoodIdx], needsMoodIdx);
      Serial.printf("Idle: %s (H:%d E:%d B:%d Joy:%d)\n",
        currentMoodName, (int)needs.getHunger(), (int)needs.getEnergy(),
        (int)needs.getBoredom(), (int)needs.getHappiness());
    }
  }

  // Render
  eye->update();

  // Mood name overlay
  canvas->setTextSize(2);
  canvas->setTextColor(0xFFFF);
  canvas->setTextWrap(false);
  int16_t x1, y1;
  uint16_t tw, th;
  canvas->getTextBounds(currentMoodName, 0, 0, &x1, &y1, &tw, &th);
  int16_t tx = (240 - tw) / 2 - x1;
  int16_t ty = 205;
  for (int8_t dx = -1; dx <= 1; dx++) {
    for (int8_t dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0) continue;
      canvas->setCursor(tx + dx, ty + dy);
      canvas->setTextColor(0x0000);
      canvas->print(currentMoodName);
    }
  }
  canvas->setCursor(tx, ty);
  canvas->setTextColor(0xFFFF);
  canvas->print(currentMoodName);

  // Connection indicator (tiny dot top-right)
  uint16_t dotColor = mqtt.isConnected() ? 0x07E0 : (mqtt.isWifiConnected() ? 0xFFE0 : 0xF800);
  canvas->fillCircle(220, 20, 3, dotColor);

  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);

  // Publish status periodically
  int8_t p, a, d;
  eye->getCurrentPAD(p, a, d);
  mqtt.publishStatus(p, a, d, currentMoodName, millis());
}
