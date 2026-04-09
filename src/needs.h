#ifndef NEEDS_H
#define NEEDS_H

#include <Arduino.h>
#include "moods.h"

class NeedsEngine {
public:
  NeedsEngine();

  // Call every loop iteration
  void update(unsigned long nowMs);

  // External events (from MQTT/HA interactions)
  void feed();          // resets hunger
  void pet();           // boosts happiness, resets loneliness
  void entertain();     // resets boredom
  void onMqttCommand(); // any MQTT command counts as interaction

  // Get the current autonomous mood (blended from needs + time)
  // Returns a mood index, or -1 to use raw PAD
  int getCurrentMoodIndex();

  // Get raw PAD from needs state (when no preset fits)
  void getCurrentPAD(int8_t &p, int8_t &a, int8_t &d);

  // How long since last MQTT command (ms)
  unsigned long timeSinceInteraction() { return millis() - lastInteractionMs; }

  // Should the needs engine control the face? (true when idle)
  bool shouldControl() { return timeSinceInteraction() > idleTimeoutMs; }

  // Get needs for status publishing
  uint8_t getHunger()    { return hunger; }
  uint8_t getEnergy()    { return energy; }
  uint8_t getBoredom()   { return boredom; }
  uint8_t getHappiness() { return happiness; }

private:
  // Needs: 0 = fully satisfied, 100 = critical
  float hunger;     // increases over time, reset by feed()
  float boredom;    // increases over time, reset by entertain() / interactions
  float loneliness; // increases when no interactions

  // Positive state: 100 = max, 0 = empty
  float energy;     // decreases during day, resets at "night"
  float happiness;  // decreases slowly, boosted by interactions

  unsigned long lastUpdateMs;
  unsigned long lastInteractionMs;
  unsigned long lastMoodChangeMs;
  int currentMoodIdx;

  static const unsigned long idleTimeoutMs = 60000; // 60s before needs take over

  // Time-of-day awareness (uses millis as proxy, or real time if available)
  int getHourOfDay();

  // Pick best mood from needs state
  void recalculateMood();
};

#endif
