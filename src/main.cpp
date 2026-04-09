#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "moods.h"
#include "moods_data.h"
#include "eye_renderer.h"

// XIAO ESP32-C6 -> GC9A01 pin mapping
#define TFT_DC    0   // D0 = GPIO 0
#define TFT_CS    1   // D1 = GPIO 1
#define TFT_RST   2   // D2 = GPIO 2

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
uint16_t *framebuffer = NULL;
EyeRenderer *eye = NULL;

int currentMood = 0;
unsigned long moodTimer = 0;
#define MOOD_DISPLAY_MS 3000  // 3 seconds per mood

void setup() {
  Serial.begin(115200);
  delay(500);

  framebuffer = (uint16_t *)malloc(240 * 240 * sizeof(uint16_t));
  if (!framebuffer) {
    Serial.println("FRAMEBUFFER ALLOC FAILED!");
    while (1) delay(1000);
  }
  memset(framebuffer, 0, 240 * 240 * sizeof(uint16_t));
  Serial.printf("Framebuffer OK. Free heap: %d\n", ESP.getFreeHeap());

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(0x0000);

  eye = new EyeRenderer(framebuffer, 240, 240);

  // Start with first mood
  eye->setMoodImmediate(MOODS[0]);
  moodTimer = millis();

  Serial.printf("Mood demo: cycling through %d moods\n", NUM_MOODS);
  Serial.printf("Starting: %s\n", MOODS[0].name);
}

void loop() {
  // Cycle to next mood
  if (millis() - moodTimer > MOOD_DISPLAY_MS) {
    moodTimer = millis();
    currentMood = (currentMood + 1) % NUM_MOODS;
    eye->setMood(MOODS[currentMood]);
    Serial.printf("[%3d] %s\n", currentMood, MOODS[currentMood].name);
  }

  // Render frame
  eye->update();

  // Push to display
  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
}
