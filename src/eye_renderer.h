#ifndef EYE_RENDERER_H
#define EYE_RENDERER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "moods.h"

class EyeRenderer {
public:
  EyeRenderer(uint16_t *framebuffer, int16_t width, int16_t height);

  // Set target mood — renderer will tween toward it
  void setMood(const MoodParams &mood);
  void setMoodImmediate(const MoodParams &mood);

  // Call every frame — updates animation and renders to framebuffer
  void update();

  // Force a blink
  void blink();

  // Get current mood name
  const char* getCurrentMoodName() { return current.name; }

private:
  uint16_t *fb;
  int16_t w, h;
  int16_t cx, cy; // center

  // Current interpolated state and target
  MoodParams current;
  MoodParams target;
  float tweenProgress; // 0.0 = current, 1.0 = target reached
  float tweenSpeed;    // how fast to tween (0.02 = slow, 0.1 = fast)

  // Animation state
  float blinkTimer;
  float blinkState;       // 0.0 = open, 1.0 = closed
  bool  blinking;
  float saccadeX, saccadeY;
  float saccadeTimer;
  float bouncePhase;
  unsigned long lastFrameMs;

  // Drawing helpers
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color);
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void fillScreen(uint16_t color);

  // Interpolation
  uint8_t lerpU8(uint8_t a, uint8_t b, float t);
  int8_t  lerpI8(int8_t a, int8_t b, float t);
  uint16_t lerpColor(uint16_t a, uint16_t b, float t);
  void lerpMood(MoodParams &out, const MoodParams &a, const MoodParams &b, float t);
};

#endif // EYE_RENDERER_H
