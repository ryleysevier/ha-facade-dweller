#ifndef EMOJI_FX_H
#define EMOJI_FX_H

#include <Arduino.h>
#include "icons.h"
#include "moods.h"

// Emoji animation pattern types
enum EmojiPattern {
  EMOJI_NONE = 0,
  EMOJI_PUPIL_REPLACE,   // Replace pupils with icon
  EMOJI_FLOAT_ABOVE,     // Icon floats/bobs above eyes
  EMOJI_RAIN_DOWN,       // Multiple icons fall from top
  EMOJI_ORBIT,           // Icons circle around the face
  EMOJI_EYE_SPARKLE,     // Tiny icons at specular highlight spots
  EMOJI_BOTTOM_STATUS,   // Icon sits below eyes
  EMOJI_BG_FILL,         // Scattered icons in background
  EMOJI_SIDE_PEEK,       // Icon peeks in from screen edge
  EMOJI_PULSE_CENTER,    // Icon pulses between eyes
  EMOJI_TEAR_DROP,       // Icon attached to eye corner
};

// Active emoji effect config
struct EmojiEffect {
  EmojiPattern pattern;
  IconId icon;
  IconId icon2;       // optional second icon (e.g. cloud + drops)
  uint16_t color;     // icon tint color
};

// Mood → emoji mapping
// Returns true if this mood has an emoji effect
bool getMoodEmoji(int moodIndex, EmojiEffect &effect);

class EmojiFx {
public:
  EmojiFx(uint16_t *framebuffer, int16_t w, int16_t h);

  void setEffect(const EmojiEffect &effect);
  void clearEffect();

  // Call each frame after eyes are drawn. Pass eye geometry for positioning.
  void update(float dt, int16_t eyeLx, int16_t eyeRx, int16_t eyeY,
              int16_t eyeW, int16_t eyeH, int16_t pupilLx, int16_t pupilRx, int16_t pupilY);

private:
  uint16_t *fb;
  int16_t scrW, scrH, cx, cy;
  EmojiEffect active;
  float phase;       // animation phase
  float particles[8][2]; // for rain/orbit/bg positions

  void drawIcon(int16_t x, int16_t y, const uint8_t *icon, uint16_t color, int16_t scale = 1);
  void drawIconCentered(int16_t cx, int16_t cy, const uint8_t *icon, uint16_t color, int16_t scale = 1);

  void fxPupilReplace(int16_t pupilLx, int16_t pupilRx, int16_t pupilY, int16_t eyeH);
  void fxFloatAbove(float dt, int16_t eyeY);
  void fxRainDown(float dt);
  void fxOrbit(float dt);
  void fxEyeSparkle(float dt, int16_t pupilLx, int16_t pupilRx, int16_t pupilY);
  void fxBottomStatus(int16_t eyeY, int16_t eyeH);
  void fxBgFill(float dt);
  void fxSidePeek(float dt);
  void fxPulseCenter(float dt);
  void fxTearDrop(float dt, int16_t eyeLx, int16_t eyeRx, int16_t eyeY, int16_t eyeW, int16_t eyeH);
};

#endif
