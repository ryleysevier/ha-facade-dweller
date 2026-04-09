#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "moods.h"
#include "moods_data.h"
#include "eye_renderer.h"

// XIAO ESP32-C6 -> GC9A01
#define TFT_DC    0
#define TFT_CS    1
#define TFT_RST   2

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
uint16_t *framebuffer = NULL;
GFXcanvas16 *canvas = NULL;
EyeRenderer *eye = NULL;

int currentMood = 0;
unsigned long moodTimer = 0;
#define MOOD_DISPLAY_MS 4000

void setup() {
  Serial.begin(115200);
  delay(500);

  // Use GFXcanvas16 which allocates its own buffer
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
  eye->setMood(MOODS[0], 0);
  moodTimer = millis();

  Serial.printf("PAD Emotion Demo — %d moods\n", NUM_MOODS);
}

void loop() {
  if (millis() - moodTimer > MOOD_DISPLAY_MS) {
    moodTimer = millis();
    currentMood = (currentMood + 1) % NUM_MOODS;
    eye->setMood(MOODS[currentMood], currentMood);
    const Mood &m = MOODS[currentMood];
    Serial.printf("[%3d] %-20s P:%4d A:%4d D:%4d\n", currentMood, m.name, m.pleasure, m.arousal, m.dominance);
  }

  // Render eye to framebuffer
  eye->update();

  // Draw mood name at bottom of screen
  const Mood &m = MOODS[currentMood];
  canvas->setTextSize(2);
  canvas->setTextColor(0xFFFF);
  canvas->setTextWrap(false);

  // Measure text to center it
  int16_t x1, y1;
  uint16_t tw, th;
  canvas->getTextBounds(m.name, 0, 0, &x1, &y1, &tw, &th);
  int16_t tx = (240 - tw) / 2 - x1;
  int16_t ty = 205;

  // Draw text with dark outline for readability
  for (int8_t dx = -1; dx <= 1; dx++) {
    for (int8_t dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0) continue;
      canvas->setCursor(tx + dx, ty + dy);
      canvas->setTextColor(0x0000);
      canvas->print(m.name);
    }
  }
  canvas->setCursor(tx, ty);
  canvas->setTextColor(0xFFFF);
  canvas->print(m.name);

  // Push to display
  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
}
