#include "emoji_fx.h"
#include <math.h>

// --- Mood → Emoji mapping ---

bool getMoodEmoji(int moodIndex, EmojiEffect &effect) {
  effect.icon2 = ICON_ID_COUNT; // no second icon by default
  effect.color = 0xFFFF;        // white default

  switch (moodIndex) {
    case 9:   // love
      effect = {EMOJI_PUPIL_REPLACE, ICON_ID_HEART, ICON_ID_COUNT, 0xF800}; // red hearts
      return true;
    case 180: // stargazing
      effect = {EMOJI_PUPIL_REPLACE, ICON_ID_STAR, ICON_ID_COUNT, 0xFFE0}; // yellow stars
      return true;
    case 98:  // rain_detected
      effect = {EMOJI_RAIN_DOWN, ICON_ID_DROP, ICON_ID_CLOUD, 0x5D7F}; // light blue
      return true;
    case 102: // snowing
      effect = {EMOJI_RAIN_DOWN, ICON_ID_SNOW, ICON_ID_COUNT, 0xFFFF}; // white
      return true;
    case 33:  // music_playing
      effect = {EMOJI_BG_FILL, ICON_ID_NOTE, ICON_ID_COUNT, 0xD69A}; // light grey
      return true;
    case 18:  // confused
      effect = {EMOJI_ORBIT, ICON_ID_QUESTION, ICON_ID_COUNT, 0xFFE0}; // yellow
      return true;
    case 45:  // alarm_triggered
      effect = {EMOJI_PULSE_CENTER, ICON_ID_WARN, ICON_ID_COUNT, 0xFBE0}; // orange
      return true;
    case 132: // morning_coffee
      effect = {EMOJI_BOTTOM_STATUS, ICON_ID_MUG, ICON_ID_COUNT, 0xC460}; // brown
      return true;
    case 22:  // too_hot
      effect = {EMOJI_TEAR_DROP, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F}; // sweat
      return true;
    case 164: // birthday
      effect = {EMOJI_BG_FILL, ICON_ID_PARTY, ICON_ID_COUNT, 0xFFE0}; // confetti
      return true;
    case 90:  // doorbell
      effect = {EMOJI_SIDE_PEEK, ICON_ID_BELL, ICON_ID_COUNT, 0xFFE0}; // yellow bell
      return true;
    case 1:   // sad
      effect = {EMOJI_TEAR_DROP, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F}; // teardrop
      return true;
    case 89:  // napping
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0xBDF7}; // light grey
      return true;
    case 57:  // package_here
      effect = {EMOJI_SIDE_PEEK, ICON_ID_BOX, ICON_ID_COUNT, 0xC460}; // brown box
      return true;
    case 184: // thunderstorm
      effect = {EMOJI_RAIN_DOWN, ICON_ID_DROP, ICON_ID_BOLT, 0x5D7F};
      return true;
    case 6:   // excited
      effect = {EMOJI_EYE_SPARKLE, ICON_ID_STAR, ICON_ID_COUNT, 0xFFE0};
      return true;
    case 109: // water_leak
      effect = {EMOJI_RAIN_DOWN, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F};
      return true;
    case 59:  // smoke_detected
      effect = {EMOJI_PULSE_CENTER, ICON_ID_WARN, ICON_ID_COUNT, 0xF800}; // red warning
      return true;
    case 15:  // nervous
      effect = {EMOJI_TEAR_DROP, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F}; // sweat
      return true;
    case 96:  // party_mode
      effect = {EMOJI_BG_FILL, ICON_ID_PARTY, ICON_ID_COUNT, 0xF81F}; // magenta confetti
      return true;

    // --- NEW: differentiating similar moods with emojis ---

    // Weather differentiation
    case 100: // sunny
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_STAR, ICON_ID_COUNT, 0xFFE0}; // sun-like star above
      return true;
    case 101: // cloudy
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_CLOUD, ICON_ID_COUNT, 0xBDF7}; // grey cloud
      return true;
    case 103: // windy
      effect = {EMOJI_ORBIT, ICON_ID_CLOUD, ICON_ID_COUNT, 0xBDF7}; // clouds swirling
      return true;
    case 185: // snowfall
      effect = {EMOJI_RAIN_DOWN, ICON_ID_SNOW, ICON_ID_COUNT, 0xFFFF};
      return true;
    case 186: // wind_howling
      effect = {EMOJI_ORBIT, ICON_ID_CLOUD, ICON_ID_COUNT, 0x9CF3};
      return true;
    case 187: // heatwave
      effect = {EMOJI_TEAR_DROP, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F}; // sweat
      return true;

    // Home utility differentiation
    case 48: // heating_on
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_STAR, ICON_ID_COUNT, 0xFBE0}; // warm glow
      return true;
    case 49: // cooling_on
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_SNOW, ICON_ID_COUNT, 0x5D7F}; // cool snowflake
      return true;
    case 50: // vacuum_running
      effect = {EMOJI_ORBIT, ICON_ID_STAR, ICON_ID_COUNT, 0xBDF7}; // spinning dust
      return true;
    case 43: // washer_running
      effect = {EMOJI_ORBIT, ICON_ID_DROP, ICON_ID_COUNT, 0x5D7F}; // spinning water
      return true;
    case 52: // battery_low
      effect = {EMOJI_PULSE_CENTER, ICON_ID_BOLT, ICON_ID_COUNT, 0xF800}; // red bolt pulsing
      return true;
    case 53: // battery_full
      effect = {EMOJI_BOTTOM_STATUS, ICON_ID_BOLT, ICON_ID_COUNT, 0x07E0}; // green bolt
      return true;
    case 54: // internet_down
      effect = {EMOJI_PULSE_CENTER, ICON_ID_WARN, ICON_ID_COUNT, 0xFBE0};
      return true;

    // Holiday differentiation
    case 160: // christmas
      effect = {EMOJI_BG_FILL, ICON_ID_STAR, ICON_ID_COUNT, 0xFFE0}; // stars
      return true;
    case 161: // halloween
      effect = {EMOJI_BG_FILL, ICON_ID_STAR, ICON_ID_COUNT, 0xFBE0}; // orange stars
      return true;
    case 162: // valentines
      effect = {EMOJI_PUPIL_REPLACE, ICON_ID_HEART, ICON_ID_COUNT, 0xF81F}; // pink hearts
      return true;
    case 163: // new_years
      effect = {EMOJI_BG_FILL, ICON_ID_PARTY, ICON_ID_COUNT, 0xFFE0};
      return true;
    case 165: // fireworks
      effect = {EMOJI_EYE_SPARKLE, ICON_ID_STAR, ICON_ID_COUNT, 0xFFFF};
      return true;

    // Time-of-day differentiation
    case 66:  // tired
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0x9CF3};
      return true;
    case 67:  // exhausted
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0x7BEF};
      return true;
    case 95:  // everyone_asleep
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0x9CF3};
      return true;
    case 129: // goodnight
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0xBDF7};
      return true;
    case 154: // naptime
      effect = {EMOJI_FLOAT_ABOVE, ICON_ID_ZZZ, ICON_ID_COUNT, 0x9CF3};
      return true;

    // Alert/notification differentiation
    case 104: // motion_detected
      effect = {EMOJI_PULSE_CENTER, ICON_ID_WARN, ICON_ID_COUNT, 0xFFE0};
      return true;
    case 105: // phone_ringing
      effect = {EMOJI_SIDE_PEEK, ICON_ID_BELL, ICON_ID_COUNT, 0xBDF7};
      return true;
    case 110: // co2_high
      effect = {EMOJI_PULSE_CENTER, ICON_ID_WARN, ICON_ID_COUNT, 0xFBE0};
      return true;
    case 111: // noise_alert
      effect = {EMOJI_PULSE_CENTER, ICON_ID_BELL, ICON_ID_COUNT, 0xFBE0};
      return true;
    case 114: // power_outage
      effect = {EMOJI_PULSE_CENTER, ICON_ID_BOLT, ICON_ID_COUNT, 0xF800};
      return true;
    case 56:  // mail_arrived
      effect = {EMOJI_SIDE_PEEK, ICON_ID_BOX, ICON_ID_COUNT, 0xBDF7};
      return true;

    // Activity differentiation
    case 155: // workout_time
      effect = {EMOJI_EYE_SPARKLE, ICON_ID_STAR, ICON_ID_COUNT, 0x07E0}; // green sparkle
      return true;
    case 173: // gaming
      effect = {EMOJI_EYE_SPARKLE, ICON_ID_STAR, ICON_ID_COUNT, 0x07E0};
      return true;
    case 159: // tea_time
      effect = {EMOJI_BOTTOM_STATUS, ICON_ID_MUG, ICON_ID_COUNT, 0xBDF7};
      return true;
    case 183: // campfire
      effect = {EMOJI_BOTTOM_STATUS, ICON_ID_STAR, ICON_ID_COUNT, 0xFBE0}; // warm glow below
      return true;
    case 174: // reading
      effect = {EMOJI_BOTTOM_STATUS, ICON_ID_BOX, ICON_ID_COUNT, 0xC460}; // book-ish
      return true;

    default:
      return false;
  }
}

// --- Constructor ---

EmojiFx::EmojiFx(uint16_t *framebuffer, int16_t w, int16_t h)
  : fb(framebuffer), scrW(w), scrH(h), cx(w/2), cy(h/2), phase(0) {
  active.pattern = EMOJI_NONE;
  // Init particle positions
  for (int i = 0; i < 8; i++) {
    particles[i][0] = random(0, w);
    particles[i][1] = random(0, h);
  }
}

void EmojiFx::setEffect(const EmojiEffect &effect) {
  active = effect;
  phase = 0;
  for (int i = 0; i < 8; i++) {
    particles[i][0] = random(20, scrW - 20);
    particles[i][1] = random(-20, 0);
  }
}

void EmojiFx::clearEffect() {
  active.pattern = EMOJI_NONE;
}

// --- Icon drawing ---

void EmojiFx::drawIcon(int16_t x, int16_t y, const uint8_t *icon, uint16_t color, int16_t scale) {
  for (int16_t iy = 0; iy < ICON_H; iy++) {
    uint8_t b0 = icon[iy * 2];
    uint8_t b1 = icon[iy * 2 + 1];
    uint16_t row = (b0 << 8) | b1;
    for (int16_t ix = 0; ix < ICON_W; ix++) {
      if (row & (0x8000 >> ix)) {
        for (int16_t sy = 0; sy < scale; sy++) {
          for (int16_t sx = 0; sx < scale; sx++) {
            int16_t px = x + ix * scale + sx;
            int16_t py = y + iy * scale + sy;
            if (px >= 0 && px < scrW && py >= 0 && py < scrH) {
              fb[py * scrW + px] = color;
            }
          }
        }
      }
    }
  }
}

void EmojiFx::drawIconCentered(int16_t cx, int16_t cy, const uint8_t *icon, uint16_t color, int16_t scale) {
  drawIcon(cx - (ICON_W * scale) / 2, cy - (ICON_H * scale) / 2, icon, color, scale);
}

// --- Animation patterns ---

void EmojiFx::fxPupilReplace(int16_t pupilLx, int16_t pupilRx, int16_t pupilY, int16_t eyeH) {
  const uint8_t *icon = ICON_DATA[active.icon];
  // Scale based on eye size
  int16_t scale = max((int16_t)1, (int16_t)(eyeH / 20));
  drawIconCentered(pupilLx, pupilY, icon, active.color, scale);
  drawIconCentered(pupilRx, pupilY, icon, active.color, scale);
}

void EmojiFx::fxFloatAbove(float dt, int16_t eyeY) {
  const uint8_t *icon = ICON_DATA[active.icon];
  float bobY = sinf(phase * 2.0f) * 5.0f;
  int16_t y = eyeY - 45 + (int16_t)bobY;
  float driftX = sinf(phase * 0.7f) * 8.0f;
  drawIconCentered(cx + (int16_t)driftX + 15, y, icon, active.color, 3);
  drawIconCentered(cx + (int16_t)driftX - 15, y + 12, icon, active.color, 2);
}

void EmojiFx::fxRainDown(float dt) {
  const uint8_t *icon = ICON_DATA[active.icon];
  for (int i = 0; i < 6; i++) {
    particles[i][1] += 40.0f * dt + i * 5.0f * dt;
    if (particles[i][1] > scrH + 10) {
      particles[i][0] = random(30, scrW - 30);
      particles[i][1] = random(-30, -5);
    }
    drawIconCentered((int16_t)particles[i][0], (int16_t)particles[i][1], icon, active.color, 2);
  }
  if (active.icon2 < ICON_ID_COUNT) {
    drawIconCentered(cx, 25, ICON_DATA[active.icon2], active.color, 3);
  }
  if (active.icon2 == ICON_ID_BOLT && ((int)(phase * 3) % 7 == 0)) {
    drawIconCentered(cx + 20, 50, ICON_DATA[ICON_ID_BOLT], 0xFFFF, 3);
  }
}

void EmojiFx::fxOrbit(float dt) {
  const uint8_t *icon = ICON_DATA[active.icon];
  for (int i = 0; i < 3; i++) {
    float angle = phase * 2.0f + i * (2.0f * M_PI / 3.0f);
    int16_t ox = cx + (int16_t)(cosf(angle) * 75);
    int16_t oy = cy + (int16_t)(sinf(angle) * 55);
    int16_t scale = sinf(angle) > 0 ? 3 : 2; // front=bigger, back=smaller
    drawIconCentered(ox, oy, icon, active.color, scale);
  }
}

void EmojiFx::fxEyeSparkle(float dt, int16_t pupilLx, int16_t pupilRx, int16_t pupilY) {
  const uint8_t *icon = ICON_DATA[active.icon];
  float sparkle = (sinf(phase * 5.0f) + 1.0f) * 0.5f;
  if (sparkle > 0.6f) {
    drawIconCentered(pupilLx - 8, pupilY - 10, icon, active.color, 2);
    drawIconCentered(pupilRx - 8, pupilY - 10, icon, active.color, 2);
  }
}

void EmojiFx::fxBottomStatus(int16_t eyeY, int16_t eyeH) {
  const uint8_t *icon = ICON_DATA[active.icon];
  int16_t y = eyeY + eyeH + 20;
  float bobY = sinf(phase * 1.5f) * 2.0f;
  drawIconCentered(cx, y + (int16_t)bobY, icon, active.color, 3);
}

void EmojiFx::fxBgFill(float dt) {
  const uint8_t *icon = ICON_DATA[active.icon];
  for (int i = 0; i < 6; i++) {
    float ox = sinf(phase * 0.3f + i * 1.5f) * 10.0f;
    float oy = cosf(phase * 0.4f + i * 1.2f) * 10.0f;
    float bx = 20 + (i % 3) * 85 + ox;
    float by = 15 + (i / 3) * 185 + oy;
    drawIcon((int16_t)bx, (int16_t)by, icon, active.color, 2);
  }
}

void EmojiFx::fxSidePeek(float dt) {
  const uint8_t *icon = ICON_DATA[active.icon];
  float peekX = sinf(min(phase * 2.0f, (float)M_PI / 2.0f));
  int16_t x = scrW - (int16_t)(peekX * 45);
  int16_t y = cy - 25;
  drawIconCentered(x, y, icon, active.color, 3);
}

void EmojiFx::fxPulseCenter(float dt) {
  const uint8_t *icon = ICON_DATA[active.icon];
  float pulse = (sinf(phase * 4.0f) + 1.0f) * 0.5f;
  int16_t scale = 2 + (int16_t)(pulse * 2.0f); // 2x to 4x
  int16_t y = cy - 5;
  drawIconCentered(cx, y, icon, active.color, scale);
}

void EmojiFx::fxTearDrop(float dt, int16_t eyeLx, int16_t eyeRx, int16_t eyeY, int16_t eyeW, int16_t eyeH) {
  const uint8_t *icon = ICON_DATA[active.icon];
  // Teardrop at bottom-outer corner of left eye, slides down slowly
  float tearY = fmodf(phase * 0.5f, 1.5f);
  if (tearY < 1.0f) {
    int16_t tx = eyeLx - eyeW / 2 + 2;
    int16_t ty = eyeY + eyeH / 2 + (int16_t)(tearY * 20);
    drawIcon(tx, ty, icon, active.color, 2);
  }
}

// --- Main update ---

void EmojiFx::update(float dt, int16_t eyeLx, int16_t eyeRx, int16_t eyeY,
                      int16_t eyeW, int16_t eyeH, int16_t pupilLx, int16_t pupilRx, int16_t pupilY) {
  if (active.pattern == EMOJI_NONE) return;

  phase += dt;

  switch (active.pattern) {
    case EMOJI_PUPIL_REPLACE:
      fxPupilReplace(pupilLx, pupilRx, pupilY, eyeH);
      break;
    case EMOJI_FLOAT_ABOVE:
      fxFloatAbove(dt, eyeY);
      break;
    case EMOJI_RAIN_DOWN:
      fxRainDown(dt);
      break;
    case EMOJI_ORBIT:
      fxOrbit(dt);
      break;
    case EMOJI_EYE_SPARKLE:
      fxEyeSparkle(dt, pupilLx, pupilRx, pupilY);
      break;
    case EMOJI_BOTTOM_STATUS:
      fxBottomStatus(eyeY, eyeH);
      break;
    case EMOJI_BG_FILL:
      fxBgFill(dt);
      break;
    case EMOJI_SIDE_PEEK:
      fxSidePeek(dt);
      break;
    case EMOJI_PULSE_CENTER:
      fxPulseCenter(dt);
      break;
    case EMOJI_TEAR_DROP:
      fxTearDrop(dt, eyeLx, eyeRx, eyeY, eyeW, eyeH);
      break;
    default:
      break;
  }
}
