#ifndef MOODS_H
#define MOODS_H

#include <stdint.h>

// PAD Emotional Model
// Pleasure:  -1.0 (miserable) to +1.0 (ecstatic)
// Arousal:   -1.0 (comatose)  to +1.0 (frantic)
// Dominance: -1.0 (submissive) to +1.0 (dominant)
//
// Stored as int8_t scaled: -100 to +100 = -1.0 to +1.0

typedef struct {
  char name[20];
  int8_t pleasure;   // -100 to 100
  int8_t arousal;    // -100 to 100
  int8_t dominance;  // -100 to 100
} Mood;

#define NUM_MOODS 200

extern const Mood MOODS[];

// Helper to convert stored int8 to float -1..1
static inline float padToFloat(int8_t v) { return v / 100.0f; }

// RGB565 helper
#define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#endif // MOODS_H
