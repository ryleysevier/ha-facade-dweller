#include "needs.h"
#include <time.h>

// Mood indices from moods_data.cpp
// Basic: 0=happy 1=sad 5=content 6=excited 7=bored 8=curious 16=peaceful
// Needs: 60=slightly_hungry 61=hungry 62=starving 65=slightly_tired 66=tired
//        67=exhausted 70=slightly_bored 71=very_bored 73=playful 74=lonely
//        75=needy 77=calm 78=zen 81=attention_seek 85=snuggly 89=napping
// Time:  130=dawn 131=early_morning 132=morning_coffee 134=noon
//        135=afternoon_slump 137=golden_hour 142=relaxed_evening
//        144=late_night 145=midnight 146=deep_night

NeedsEngine::NeedsEngine()
  : hunger(20), boredom(10), loneliness(10),
    energy(80), happiness(70),
    lastUpdateMs(0), lastInteractionMs(0),
    lastMoodChangeMs(0), currentMoodIdx(5) { // start content
}

int NeedsEngine::getHourOfDay() {
  // Try to get real time
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 0)) {
    return timeinfo.tm_hour;
  }
  // Fallback: estimate from uptime (assume boot = current time)
  // This is imperfect but works for basic day/night cycle
  return -1; // unknown
}

void NeedsEngine::update(unsigned long nowMs) {
  if (lastUpdateMs == 0) {
    lastUpdateMs = nowMs;
    lastInteractionMs = nowMs;
    lastMoodChangeMs = nowMs;
    return;
  }

  float dt = (nowMs - lastUpdateMs) / 1000.0f; // seconds
  lastUpdateMs = nowMs;

  if (dt > 10.0f) dt = 10.0f; // clamp after sleep/pause

  int hour = getHourOfDay();

  // --- Need decay rates (per second) ---

  // Hunger: builds up slowly (~2 hours to go from 0→100)
  hunger += dt * 0.014f;

  // Boredom: builds faster when alone (~30 min to go 0→100)
  float boredomRate = 0.055f;
  if (timeSinceInteraction() > 300000) boredomRate = 0.08f; // faster after 5 min alone
  boredom += dt * boredomRate;

  // Loneliness: builds when no interactions (~1 hour to go 0→100)
  if (timeSinceInteraction() > 120000) { // starts after 2 min
    loneliness += dt * 0.028f;
  }

  // Energy: depends on time of day
  if (hour >= 0) {
    if (hour >= 7 && hour < 22) {
      // Daytime: slow energy drain
      energy -= dt * 0.008f;
    } else {
      // Nighttime: energy recovers (sleeping)
      energy += dt * 0.02f;
    }
  } else {
    // Unknown time: slow drain
    energy -= dt * 0.005f;
  }

  // Happiness: decays slowly, faster when needs are high
  float happyDecay = 0.003f;
  if (hunger > 70) happyDecay += 0.005f;
  if (boredom > 70) happyDecay += 0.005f;
  if (loneliness > 60) happyDecay += 0.008f;
  happiness -= dt * happyDecay;

  // Clamp all values
  hunger     = constrain(hunger, 0, 100);
  boredom    = constrain(boredom, 0, 100);
  loneliness = constrain(loneliness, 0, 100);
  energy     = constrain(energy, 0, 100);
  happiness  = constrain(happiness, 0, 100);

  // Recalculate mood every 10 seconds
  if (nowMs - lastMoodChangeMs > 10000) {
    lastMoodChangeMs = nowMs;
    recalculateMood();
  }
}

void NeedsEngine::recalculateMood() {
  int hour = getHourOfDay();

  // --- Priority system: critical needs override everything ---

  // Starving
  if (hunger > 85) { currentMoodIdx = 62; return; } // starving
  // Exhausted
  if (energy < 10) { currentMoodIdx = 67; return; } // exhausted

  // --- High needs ---
  if (hunger > 65) { currentMoodIdx = 61; return; } // hungry
  if (energy < 25) { currentMoodIdx = 66; return; } // tired

  // --- Loneliness / attention ---
  if (loneliness > 80) { currentMoodIdx = 81; return; } // attention_seek
  if (loneliness > 60) { currentMoodIdx = 75; return; } // needy
  if (loneliness > 40) { currentMoodIdx = 74; return; } // lonely

  // --- Boredom ---
  if (boredom > 80) { currentMoodIdx = 71; return; } // very_bored
  if (boredom > 50) { currentMoodIdx = 7; return; }  // bored
  if (boredom > 30) { currentMoodIdx = 70; return; } // slightly_bored

  // --- Mild hunger/tiredness ---
  if (hunger > 40) { currentMoodIdx = 60; return; } // slightly_hungry
  if (energy < 40) { currentMoodIdx = 65; return; } // slightly_tired

  // --- Time-of-day ambient moods ---
  if (hour >= 0) {
    if (hour >= 0 && hour < 5)   { currentMoodIdx = 146; return; } // deep_night
    if (hour >= 5 && hour < 7)   { currentMoodIdx = 130; return; } // dawn
    if (hour >= 7 && hour < 8)   { currentMoodIdx = 132; return; } // morning_coffee
    if (hour >= 8 && hour < 10)  { currentMoodIdx = 131; return; } // early_morning
    if (hour >= 10 && hour < 12) { currentMoodIdx = 8; return; }   // curious
    if (hour >= 12 && hour < 13) { currentMoodIdx = 134; return; } // noon
    if (hour >= 13 && hour < 15) { currentMoodIdx = 135; return; } // afternoon_slump
    if (hour >= 15 && hour < 17) { currentMoodIdx = 5; return; }   // content
    if (hour >= 17 && hour < 19) { currentMoodIdx = 137; return; } // golden_hour
    if (hour >= 19 && hour < 21) { currentMoodIdx = 142; return; } // relaxed_evening
    if (hour >= 21 && hour < 23) { currentMoodIdx = 144; return; } // late_night
    if (hour >= 23)              { currentMoodIdx = 145; return; } // midnight
  }

  // --- Fallback: mood based on happiness ---
  if (happiness > 80) { currentMoodIdx = 0; return; }  // happy
  if (happiness > 60) { currentMoodIdx = 5; return; }  // content
  if (happiness > 40) { currentMoodIdx = 77; return; } // calm
  if (happiness > 20) { currentMoodIdx = 87; return; } // mellow
  currentMoodIdx = 1; // sad
}

// --- External events ---

void NeedsEngine::feed() {
  hunger = max(0.0f, hunger - 50.0f);
  happiness = min(100.0f, happiness + 15.0f);
  lastInteractionMs = millis();
}

void NeedsEngine::pet() {
  loneliness = max(0.0f, loneliness - 40.0f);
  happiness = min(100.0f, happiness + 20.0f);
  boredom = max(0.0f, boredom - 15.0f);
  lastInteractionMs = millis();
}

void NeedsEngine::entertain() {
  boredom = max(0.0f, boredom - 50.0f);
  happiness = min(100.0f, happiness + 10.0f);
  lastInteractionMs = millis();
}

void NeedsEngine::onMqttCommand() {
  // Any MQTT command counts as interaction
  loneliness = max(0.0f, loneliness - 20.0f);
  boredom = max(0.0f, boredom - 10.0f);
  lastInteractionMs = millis();
}

int NeedsEngine::getCurrentMoodIndex() {
  return currentMoodIdx;
}

void NeedsEngine::getCurrentPAD(int8_t &p, int8_t &a, int8_t &d) {
  // Derive PAD from needs state directly
  p = (int8_t)(happiness - 50); // 0-100 → -50 to +50
  a = (int8_t)((boredom > 50 ? -boredom * 0.5f : energy * 0.3f) - 20);
  d = (int8_t)(energy * 0.3f - loneliness * 0.3f);
}
