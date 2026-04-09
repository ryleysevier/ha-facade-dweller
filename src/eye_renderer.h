#ifndef EYE_RENDERER_H
#define EYE_RENDERER_H

#include <Arduino.h>
#include "moods.h"
#include "emoji_fx.h"

// Derived eye parameters — computed from PAD values
struct EyeParams {
  // Shape
  float width;          // 80-180
  float height;         // 80-180
  float roundness;      // 0.0 (rect) to 1.0 (circle)
  float openness;       // 0.0 (closed) to 1.0 (wide open)
  // Pupil
  float pupilSize;      // 0.2 to 0.8 (fraction of eye)
  float gazeX;          // -1.0 to 1.0
  float gazeY;          // -1.0 to 1.0
  // Lids
  float lidTop;         // 0.0 (open) to 1.0 (fully drooped)
  float lidBottom;      // 0.0 (open) to 1.0 (fully raised)
  float lidAngle;       // -1.0 (inner up/angry) to 1.0 (outer up/sad)
  // Brows
  float browHeight;     // -0.5 (low, cutting into eye) to 1.0 (raised high)
  float browAngle;      // -1.0 (inner down/angry) to 1.0 (inner up/worried)
  float browThickness;  // 0.3 to 1.0
  float browCurve;      // -1.0 (frown/sagging) to 1.0 (arched up)
  // Colors
  float hue;            // 0-360
  float saturation;     // 0-1
  float brightness;     // 0-1
  float glowIntensity;  // 0-1
  // Timing
  float blinkRate;      // seconds between blinks
  float moveSpeed;      // how fast eyes move to new positions
  float saccadeAmount;  // micro-movement intensity
};

class EyeRenderer {
public:
  EyeRenderer(uint16_t *framebuffer, int16_t width, int16_t height);

  // Set emotion via PAD values (-1.0 to 1.0 each)
  void setEmotion(float pleasure, float arousal, float dominance);

  // Set from a Mood struct (with index for emoji lookup)
  void setMood(const Mood &mood, int moodIndex = -1);

  // Call every frame
  void update();

  // Force blink
  void blink();

private:
  uint16_t *fb;
  int16_t scrW, scrH, cx, cy;
  EmojiFx *emojiFx;
  int currentMoodIndex;
  bool pupilReplaced; // true when emoji replaces pupil

  // Current and target eye params (for tweening)
  EyeParams current;
  EyeParams target;

  // Animation state
  float tweenT;           // 0..1 progress toward target
  unsigned long lastMs;
  float blinkTimer;
  float blinkPhase;       // 0=open, goes 0→1→0 during blink
  bool  isBlinking;
  float saccadeX, saccadeY;
  float saccadeTimer;
  float breathPhase;
  float gazeTargetX, gazeTargetY;
  float gazeCurrentX, gazeCurrentY;
  float gazeTimer;

  // PAD → EyeParams mapping
  void padToEyeParams(float p, float a, float d, EyeParams &out);

  // Color from PAD
  uint16_t emotionColor(float p, float a, float d);
  uint16_t glowColor(float p, float a, float d);
  uint16_t bgColor(float p, float a, float d);

  // Easing functions
  static float easeInOut(float t);
  static float easeOut(float t);
  static float easeIn(float t);

  // Lerp
  static float lerp(float a, float b, float t);
  void lerpParams(EyeParams &out, const EyeParams &a, const EyeParams &b, float t);
  uint16_t lerpColor565(uint16_t a, uint16_t b, float t);

  // HSV to RGB565
  uint16_t hsv565(float h, float s, float v);

  // Drawing primitives (direct to framebuffer)
  void fbFillScreen(uint16_t color);
  void fbFillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color);
  void fbFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
  void fbFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fbFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
};

#endif
