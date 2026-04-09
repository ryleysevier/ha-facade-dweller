#ifndef MOODS_H
#define MOODS_H

#include <stdint.h>

typedef struct {
  char name[24];           // e.g. "cozy_evening"
  // Eye shape
  uint8_t eye_width;       // 60-180
  uint8_t eye_height;      // 60-180
  uint8_t border_radius;   // 10-90
  uint8_t openness;        // 0 (closed) - 100 (wide open)
  // Pupil
  uint8_t pupil_size;      // 10-60
  int8_t  pupil_x;         // -40 to 40 (horizontal offset)
  int8_t  pupil_y;         // -40 to 40 (vertical offset)
  // Eyelids
  uint8_t lid_top;         // 0-50 (droop from top)
  uint8_t lid_bottom;      // 0-50 (raise from bottom)
  int8_t  lid_angle;       // -30 to 30 (negative=angry, positive=tired)
  // Colors (RGB565)
  uint16_t eye_color;      // main eye fill color
  uint16_t bg_color;       // background color
  uint16_t glow_color;     // glow/aura color behind eye
  uint8_t  glow_radius;    // 0-60 (0 = no glow)
  // Animation parameters
  uint8_t blink_interval;  // seconds between blinks (2-15)
  uint8_t blink_variation; // random variation in seconds (0-5)
  uint8_t saccade_speed;   // micro-movement speed 0-10 (0=still, 10=jittery)
  uint8_t idle_drift;      // how much the eye wanders 0-10
  uint8_t bounce_amp;      // breathing/bounce amplitude 0-10
  uint8_t bounce_speed;    // breathing/bounce speed 0-10
} MoodParams;

#define NUM_MOODS 200

// Generated mood data — will be populated by tools/generate_moods.py
extern const MoodParams MOODS[NUM_MOODS];

// Mood categories for easy lookup
#define MOOD_CAT_BASIC     0    // 0-19: basic emotions
#define MOOD_CAT_HOME      20   // 20-59: home states
#define MOOD_CAT_NEEDS     60   // 60-89: pet needs
#define MOOD_CAT_REACTIVE  90   // 90-129: reactive to events
#define MOOD_CAT_TIME      130  // 130-159: time-based
#define MOOD_CAT_SPECIAL   160  // 160-199: special/seasonal

// Helper to convert RGB to RGB565
#define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#endif // MOODS_H
