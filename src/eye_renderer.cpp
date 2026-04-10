#include "eye_renderer.h"
#include <math.h>

// --- Easing ---

float EyeRenderer::easeInOut(float t) {
  return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2) / 2.0f;
}

float EyeRenderer::easeOut(float t) {
  return 1.0f - (1.0f - t) * (1.0f - t);
}

float EyeRenderer::easeIn(float t) {
  return t * t;
}

float EyeRenderer::lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

void EyeRenderer::lerpParams(EyeParams &out, const EyeParams &a, const EyeParams &b, float t) {
  out.width         = lerp(a.width, b.width, t);
  out.height        = lerp(a.height, b.height, t);
  out.roundness     = lerp(a.roundness, b.roundness, t);
  out.openness      = lerp(a.openness, b.openness, t);
  out.pupilSize     = lerp(a.pupilSize, b.pupilSize, t);
  out.gazeX         = lerp(a.gazeX, b.gazeX, t);
  out.gazeY         = lerp(a.gazeY, b.gazeY, t);
  out.lidTop        = lerp(a.lidTop, b.lidTop, t);
  out.lidBottom     = lerp(a.lidBottom, b.lidBottom, t);
  out.lidAngle      = lerp(a.lidAngle, b.lidAngle, t);
  out.browHeight    = lerp(a.browHeight, b.browHeight, t);
  out.browAngle     = lerp(a.browAngle, b.browAngle, t);
  out.browThickness = lerp(a.browThickness, b.browThickness, t);
  out.browCurve     = lerp(a.browCurve, b.browCurve, t);
  out.hue           = lerp(a.hue, b.hue, t);
  out.saturation    = lerp(a.saturation, b.saturation, t);
  out.brightness    = lerp(a.brightness, b.brightness, t);
  out.glowIntensity = lerp(a.glowIntensity, b.glowIntensity, t);
  out.blinkRate     = lerp(a.blinkRate, b.blinkRate, t);
  out.moveSpeed     = lerp(a.moveSpeed, b.moveSpeed, t);
  out.saccadeAmount = lerp(a.saccadeAmount, b.saccadeAmount, t);
}

uint16_t EyeRenderer::lerpColor565(uint16_t a, uint16_t b, float t) {
  uint8_t r1 = (a >> 11) & 0x1F, g1 = (a >> 5) & 0x3F, b1 = a & 0x1F;
  uint8_t r2 = (b >> 11) & 0x1F, g2 = (b >> 5) & 0x3F, b2 = b & 0x1F;
  uint8_t r = r1 + (int)((r2 - r1) * t);
  uint8_t g = g1 + (int)((g2 - g1) * t);
  uint8_t bl = b1 + (int)((b2 - b1) * t);
  return (r << 11) | (g << 5) | bl;
}

// --- HSV to RGB565 ---
uint16_t EyeRenderer::hsv565(float h, float s, float v) {
  float c = v * s;
  float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
  float m = v - c;
  float r, g, b;
  if      (h < 60)  { r=c; g=x; b=0; }
  else if (h < 120) { r=x; g=c; b=0; }
  else if (h < 180) { r=0; g=c; b=x; }
  else if (h < 240) { r=0; g=x; b=c; }
  else if (h < 300) { r=x; g=0; b=c; }
  else              { r=c; g=0; b=x; }
  uint8_t r8 = (uint8_t)((r + m) * 255);
  uint8_t g8 = (uint8_t)((g + m) * 255);
  uint8_t b8 = (uint8_t)((b + m) * 255);
  return RGB565(r8, g8, b8);
}

// --- PAD → EyeParams mapping (the core intelligence) ---

void EyeRenderer::padToEyeParams(float p, float a, float d, EyeParams &out) {
  // Clamp inputs
  p = constrain(p, -1.0f, 1.0f);
  a = constrain(a, -1.0f, 1.0f);
  d = constrain(d, -1.0f, 1.0f);

  // === SQUASH & STRETCH ===
  // Surprised/scared = tall narrow (stretch), angry = wide flat (squash)
  // Calm/bored = small, excited = big
  float baseSize = 130.0f;
  out.height = baseSize + a * 35.0f + p * 10.0f;
  out.width  = baseSize + a * 15.0f + p * 10.0f + d * 10.0f;
  // Angry squash: wider, shorter
  if (p < -0.3f && a > 0.3f && d > 0.0f) {
    out.width  += 20.0f;
    out.height -= 30.0f;
  }
  // Scared stretch: taller, narrower
  if (p < -0.3f && a > 0.3f && d < -0.3f) {
    out.height += 20.0f;
    out.width  -= 10.0f;
  }

  // === SHAPE CORNERS (shape language) ===
  // Round = friendly/innocent, angular = menacing
  out.roundness = 0.65f + p * 0.25f;
  if (p < -0.3f && a > 0.3f) out.roundness = 0.2f;  // angry = sharp corners
  if (a > 0.7f && p >= 0)    out.roundness = 0.95f;  // surprised = perfectly round
  if (p > 0.5f)              out.roundness = 0.85f;  // happy = very round
  out.roundness = constrain(out.roundness, 0.15f, 1.0f);

  // === OPENNESS ===
  out.openness = 0.7f + a * 0.25f;
  if (a < -0.5f) out.openness -= 0.2f;
  if (p < -0.3f && a > 0.3f && d > 0.0f) out.openness = 0.5f; // angry squint
  if (a > 0.8f) out.openness = 1.0f; // surprised = wide open
  out.openness = constrain(out.openness, 0.12f, 1.0f);

  // === PUPIL SIZE (sclera visibility) ===
  // Small pupil in big eye = lots of white = scared/surprised
  // Big pupil filling eye = love/interest/content
  out.pupilSize = 0.35f + p * 0.08f;
  if (p > 0.5f)  out.pupilSize = 0.5f;  // love/happy = big warm pupil
  if (p < -0.3f && a > 0.5f && d < 0) out.pupilSize = 0.18f; // scared = tiny (max sclera)
  if (p < -0.3f && a > 0.3f && d > 0) out.pupilSize = 0.22f; // angry = small focused
  if (a > 0.7f && p >= 0)  out.pupilSize = 0.25f; // surprised = smallish (shows whites)
  out.pupilSize = constrain(out.pupilSize, 0.12f, 0.6f);

  // === GAZE ===
  out.gazeX = 0;
  out.gazeY = d * -0.15f;
  if (d < -0.3f) out.gazeX = -0.2f; // submissive averts gaze

  // === LID TOP ===
  out.lidTop = 0.0f;
  if (a < 0) out.lidTop = fabsf(a) * 0.5f;  // sleepy/bored droop
  if (p < -0.3f && a > 0.3f) out.lidTop = 0.45f; // angry heavy lid

  // === LID BOTTOM (happy squint) ===
  out.lidBottom = 0.0f;
  if (p > 0.3f) out.lidBottom = p * 0.4f;
  if (a > 0.7f && p >= 0) out.lidBottom = 0.0f; // surprised = no bottom squint

  // === LID ANGLE ===
  out.lidAngle = 0.0f;
  if (p < -0.3f && a > 0.3f) out.lidAngle = -0.6f - p * 0.3f; // angry V
  if (p < 0 && a < -0.3f)    out.lidAngle = 0.4f + fabsf(a) * 0.2f; // sad/tired
  if (d > 0.5f && p < 0)     out.lidAngle -= 0.2f;
  out.lidAngle = constrain(out.lidAngle, -1.0f, 1.0f);

  // COLOR: emotion → hue
  // Happy/content → warm (30-60°, gold/amber)
  // Sad → cool (220-240°, blue)
  // Angry → red (0-10°)
  // Scared → pale/desaturated
  // Calm → teal (170-190°)
  // Excited → bright warm (40-50°)
  if (p > 0.3f && a > 0.3f)       out.hue = 40;  // excited/happy: gold
  else if (p > 0.3f && a <= 0.3f)  out.hue = 30;  // content: warm amber
  else if (p < -0.3f && a > 0.3f && d > 0) out.hue = 5;   // angry: red
  else if (p < -0.3f && a > 0.3f)  out.hue = 280; // scared: purple
  else if (p < -0.3f && a <= 0)    out.hue = 220; // sad: blue
  else if (a < -0.3f)              out.hue = 180; // calm: teal
  else                              out.hue = 35;  // neutral: amber

  // Saturation: higher arousal = more saturated, fear desaturates
  out.saturation = 0.6f + fabsf(a) * 0.3f;
  if (p < -0.5f && d < -0.3f) out.saturation -= 0.2f; // fear desaturates
  out.saturation = constrain(out.saturation, 0.3f, 1.0f);

  // Brightness: pleasure brightens, sadness darkens
  out.brightness = 0.7f + p * 0.2f;
  out.brightness = constrain(out.brightness, 0.4f, 1.0f);

  // BROWS: the main expression amplifier
  // Height: arousal raises brows (surprise/alert), calm lowers them
  //         fear raises them high, anger pushes them down into the eye
  out.browHeight = 0.3f + a * 0.3f;  // arousal lifts brows
  if (p < -0.3f && a > 0.3f && d > 0) out.browHeight = -0.2f; // angry = low brows
  if (p < -0.5f && d < -0.3f) out.browHeight = 0.7f;  // scared = raised high
  out.browHeight = constrain(out.browHeight, -0.5f, 1.0f);

  // Angle: angry = inner corners down (V shape), worried = inner up (inverted V)
  out.browAngle = 0.0f;
  if (p < -0.3f && a > 0.3f) out.browAngle = -0.6f - d * 0.3f; // angry V
  if (p < -0.3f && d < -0.3f) out.browAngle = 0.5f + fabsf(d) * 0.3f; // worried inverted V
  if (p < -0.3f && a < -0.3f) out.browAngle = 0.3f; // sad = slight inner up
  if (p > 0.5f) out.browAngle = 0.1f; // happy = slight lift
  out.browAngle = constrain(out.browAngle, -1.0f, 1.0f);

  // Thickness: dominance = thicker brows, submissive = thinner
  out.browThickness = 0.5f + d * 0.2f;
  if (p < -0.3f && a > 0.5f) out.browThickness = 0.8f; // angry = thick
  out.browThickness = constrain(out.browThickness, 0.3f, 1.0f);

  // Curve: arousal arches brows up, sadness/boredom sags them
  out.browCurve = a * 0.6f + p * 0.2f;
  if (a > 0.7f) out.browCurve = 0.8f;   // very surprised = high arch
  if (a < -0.5f) out.browCurve = -0.4f;  // bored/tired = slight sag
  out.browCurve = constrain(out.browCurve, -1.0f, 1.0f);

  // Glow: high arousal = more glow, pleasure adds warmth
  out.glowIntensity = fabsf(a) * 0.5f + (p > 0 ? p * 0.3f : 0);
  out.glowIntensity = constrain(out.glowIntensity, 0.0f, 0.8f);

  // TIMING: arousal drives speed
  out.blinkRate = 5.0f - a * 2.5f; // excited=fast blinks, calm=slow
  out.blinkRate = constrain(out.blinkRate, 1.5f, 10.0f);

  out.moveSpeed = 2.0f + a * 3.0f; // excited=fast movements
  out.moveSpeed = constrain(out.moveSpeed, 0.5f, 6.0f);

  out.saccadeAmount = 0.3f + fabsf(a) * 0.5f; // high arousal = jittery
  if (a < -0.5f) out.saccadeAmount = 0.1f;     // very calm = almost still
  out.saccadeAmount = constrain(out.saccadeAmount, 0.05f, 0.8f);
}

// Color helpers
uint16_t EyeRenderer::emotionColor(float p, float a, float d) {
  EyeParams tmp;
  padToEyeParams(p, a, d, tmp);
  return hsv565(tmp.hue, tmp.saturation, tmp.brightness);
}

uint16_t EyeRenderer::glowColor(float p, float a, float d) {
  EyeParams tmp;
  padToEyeParams(p, a, d, tmp);
  return hsv565(tmp.hue, tmp.saturation * 0.5f, tmp.brightness * 0.3f);
}

uint16_t EyeRenderer::bgColor(float p, float a, float d) {
  // Subtle background tint based on emotion
  EyeParams tmp;
  padToEyeParams(p, a, d, tmp);
  return hsv565(tmp.hue, tmp.saturation * 0.15f, 0.05f);
}

// --- Constructor ---

EyeRenderer::EyeRenderer(uint16_t *framebuffer, int16_t width, int16_t height)
  : fb(framebuffer), scrW(width), scrH(height),
    cx(width / 2), cy(height / 2),
    currentMoodIndex(-1),
    tweenT(1.0f), lastMs(0),
    blinkTimer(3.0f), blinkPhase(0), isBlinking(false),
    saccadeX(0), saccadeY(0), saccadeTimer(0),
    breathPhase(0),
    gazeTargetX(0), gazeTargetY(0),
    gazeCurrentX(0), gazeCurrentY(0), gazeTimer(0),
    lookAroundTimer(3.0f), lookAroundHoldTimer(0),
    isLookingAround(false), lookAroundBaseX(0), lookAroundBaseY(0),
    curP(0), curA(0), curD(0) {
  emojiFx = new EmojiFx(framebuffer, width, height);
  memset(&current, 0, sizeof(EyeParams));
  memset(&target, 0, sizeof(EyeParams));
  // Init to neutral
  padToEyeParams(0, 0, 0, current);
  target = current;
}

// --- Public API ---

void EyeRenderer::setEmotion(float pleasure, float arousal, float dominance) {
  padToEyeParams(pleasure, arousal, dominance, target);
  tweenT = 0.0f;
}

// Category-based hue overrides so similar-PAD moods look distinct
void EyeRenderer::applyHueOverride(int moodIndex, EyeParams &params) {
  if (moodIndex < 0) return;

  // TIME-BASED (130-159): gradient from dawn pink → noon gold → dusk orange → night blue
  if (moodIndex >= 130 && moodIndex <= 159) {
    float t = (float)(moodIndex - 130) / 29.0f; // 0..1 across the day
    if (t < 0.2f)       params.hue = 320 + t * 200; // dawn: pink-ish (320-360)
    else if (t < 0.4f)  params.hue = 40 + (t - 0.2f) * 50;  // morning: gold (40-50)
    else if (t < 0.6f)  params.hue = 45;  // midday: bright gold
    else if (t < 0.8f)  params.hue = 20 + (t - 0.6f) * -75;  // evening: orange→red (20→5)
    else                 params.hue = 220 + (t - 0.8f) * 200; // night: blue (220-260)
    return;
  }

  // WEATHER moods: specific hue per weather type
  switch (moodIndex) {
    case 98:  params.hue = 210; break; // rain: steel blue
    case 99:  params.hue = 265; break; // storm: dark purple
    case 100: params.hue = 45;  params.saturation = 0.9f; break; // sunny: bright gold
    case 101: params.hue = 200; params.saturation = 0.35f; break; // cloudy: desaturated blue-grey
    case 102: params.hue = 195; params.saturation = 0.4f; break; // snowing: pale cyan
    case 103: params.hue = 175; break; // windy: teal-green
    case 184: params.hue = 270; break; // thunderstorm: vivid purple
    case 185: params.hue = 195; params.saturation = 0.35f; break; // snowfall: pale
    case 186: params.hue = 170; break; // wind howling: teal
    case 187: params.hue = 10; break;  // heatwave: red-orange
    default: break;
  }

  // HOLIDAY moods: unique hue per holiday
  switch (moodIndex) {
    case 160: params.hue = 0; params.saturation = 0.85f; break;   // christmas: red
    case 161: params.hue = 30; break;  // halloween: orange
    case 162: params.hue = 330; break; // valentines: pink
    case 163: params.hue = 50; params.saturation = 0.95f; break;  // new years: bright gold
    case 164: params.hue = 310; break; // birthday: magenta
    case 165: params.hue = 45; break;  // fireworks: gold
    case 166: params.hue = 140; break; // spring bloom: green
    case 167: params.hue = 40; break;  // summer heat: warm gold
    case 168: params.hue = 25; break;  // autumn: orange-amber
    case 169: params.hue = 200; break; // winter cold: ice blue
    default: break;
  }

  // HOME UTILITY moods: subtle tints to differentiate
  switch (moodIndex) {
    case 48: params.hue = 15; break;  // heating: warm orange-red
    case 49: params.hue = 200; break; // cooling: cool blue
    case 50: params.hue = 170; break; // vacuum: teal (busy)
    case 43: params.hue = 195; break; // washer: blue-teal
    case 58: params.hue = 150; break; // sprinklers: green
    case 37: params.hue = 170; params.saturation = 0.5f; break; // humid: murky teal
    case 38: params.hue = 40; params.saturation = 0.4f; break;  // dry air: faded amber
    default: break;
  }

  // ACTIVITY moods
  switch (moodIndex) {
    case 172: params.hue = 270; params.saturation = 0.5f; break; // meditation: soft purple
    case 173: params.hue = 120; break; // gaming: green
    case 174: params.hue = 30; break;  // reading: warm amber
    case 175: params.hue = 210; break; // working: blue (focus)
    case 176: params.hue = 290; break; // creative: purple-magenta
    case 177: params.hue = 170; params.saturation = 0.5f; break; // spa: soft teal
    case 178: params.hue = 310; break; // dance party: hot pink
    case 179: params.hue = 30; params.saturation = 0.7f; break;  // candlelight: warm
    case 180: params.hue = 240; break; // stargazing: deep blue
    case 181: params.hue = 195; break; // ocean: ocean blue
    case 182: params.hue = 140; break; // forest: green
    case 183: params.hue = 20; break;  // campfire: orange
    default: break;
  }
}

void EyeRenderer::setMood(const Mood &mood, int moodIndex) {
  curP = mood.pleasure; curA = mood.arousal; curD = mood.dominance;
  setEmotion(padToFloat(mood.pleasure), padToFloat(mood.arousal), padToFloat(mood.dominance));
  currentMoodIndex = moodIndex;
  applyHueOverride(moodIndex, target);
  pupilReplaced = false;
  EmojiEffect effect;
  if (moodIndex >= 0 && getMoodEmoji(moodIndex, effect)) {
    emojiFx->setEffect(effect);
    if (effect.pattern == EMOJI_PUPIL_REPLACE) pupilReplaced = true;
  } else {
    emojiFx->clearEffect();
  }
}

void EyeRenderer::setFace(float pleasure, float arousal, float dominance,
                           int16_t hueOverride, EmojiPattern emojiPattern,
                           IconId emojiIcon, uint16_t emojiColor) {
  curP = (int8_t)(pleasure * 100); curA = (int8_t)(arousal * 100); curD = (int8_t)(dominance * 100);
  setEmotion(pleasure, arousal, dominance);
  currentMoodIndex = -1;

  // Apply hue override
  if (hueOverride >= 0) {
    target.hue = (float)hueOverride;
  }

  // Apply emoji
  pupilReplaced = false;
  if (emojiPattern != EMOJI_NONE && emojiIcon < ICON_ID_COUNT) {
    EmojiEffect effect = {emojiPattern, emojiIcon, ICON_ID_COUNT, emojiColor};
    emojiFx->setEffect(effect);
    if (emojiPattern == EMOJI_PUPIL_REPLACE) pupilReplaced = true;
  } else {
    emojiFx->clearEffect();
  }
}

void EyeRenderer::blink() {
  isBlinking = true;
  blinkPhase = 0.0f;
}

// --- Drawing primitives ---

void EyeRenderer::fbFillScreen(uint16_t color) {
  for (int i = 0; i < scrW * scrH; i++) fb[i] = color;
}

void EyeRenderer::fbFillRect(int16_t x, int16_t y, int16_t rw, int16_t rh, uint16_t color) {
  int16_t x1 = max((int16_t)0, x), y1 = max((int16_t)0, y);
  int16_t x2 = min(scrW, (int16_t)(x + rw)), y2 = min(scrH, (int16_t)(y + rh));
  for (int16_t j = y1; j < y2; j++)
    for (int16_t i = x1; i < x2; i++)
      fb[j * scrW + i] = color;
}

void EyeRenderer::fbFillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color) {
  for (int16_t y = -r; y <= r; y++) {
    int16_t halfW = (int16_t)sqrtf(r * r - y * y);
    int16_t py = cy + y;
    if (py < 0 || py >= scrH) continue;
    int16_t x1 = max((int16_t)0, (int16_t)(cx - halfW));
    int16_t x2 = min(scrW, (int16_t)(cx + halfW + 1));
    for (int16_t x = x1; x < x2; x++)
      fb[py * scrW + x] = color;
  }
}

void EyeRenderer::fbFillRoundRect(int16_t x, int16_t y, int16_t rw, int16_t rh, int16_t r, uint16_t color) {
  r = min(r, (int16_t)(min(rw, rh) / 2));
  if (r < 1) { fbFillRect(x, y, rw, rh, color); return; }
  fbFillRect(x + r, y, rw - 2 * r, rh, color);
  fbFillRect(x, y + r, r, rh - 2 * r, color);
  fbFillRect(x + rw - r, y + r, r, rh - 2 * r, color);
  fbFillCircle(x + r, y + r, r, color);
  fbFillCircle(x + rw - r - 1, y + r, r, color);
  fbFillCircle(x + r, y + rh - r - 1, r, color);
  fbFillCircle(x + rw - r - 1, y + rh - r - 1, r, color);
}

void EyeRenderer::fbFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  if (y0 > y1) { int16_t t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }
  if (y1 > y2) { int16_t t; t=x1;x1=x2;x2=t; t=y1;y1=y2;y2=t; }
  if (y0 > y1) { int16_t t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }
  for (int16_t y = max((int16_t)0, y0); y <= min((int16_t)(scrH - 1), y2); y++) {
    float xa, xb;
    if (y < y1) {
      xa = (y1 == y0) ? x0 : x0 + (float)(x1 - x0) * (y - y0) / (y1 - y0);
      xb = (y2 == y0) ? x0 : x0 + (float)(x2 - x0) * (y - y0) / (y2 - y0);
    } else {
      xa = (y2 == y1) ? x1 : x1 + (float)(x2 - x1) * (y - y1) / (y2 - y1);
      xb = (y2 == y0) ? x0 : x0 + (float)(x2 - x0) * (y - y0) / (y2 - y0);
    }
    if (xa > xb) { float t = xa; xa = xb; xb = t; }
    int16_t ix1 = max((int16_t)0, (int16_t)xa);
    int16_t ix2 = min(scrW, (int16_t)(xb + 1));
    for (int16_t x = ix1; x < ix2; x++) fb[y * scrW + x] = color;
  }
}

// --- Main update ---

void EyeRenderer::update() {
  unsigned long now = millis();
  float dt = (lastMs == 0) ? 0.033f : (now - lastMs) / 1000.0f;
  lastMs = now;
  dt = min(dt, 0.1f);

  // --- Tween toward target ---
  EyeParams render;
  if (tweenT < 1.0f) {
    tweenT += dt * 1.5f; // ~0.7s transition
    if (tweenT > 1.0f) tweenT = 1.0f;
    float eased = easeInOut(tweenT);
    lerpParams(render, current, target, eased);
    if (tweenT >= 1.0f) current = target;
  } else {
    render = current;
  }

  // --- Autonomous blink ---
  blinkTimer -= dt;
  if (blinkTimer <= 0 && !isBlinking) {
    isBlinking = true;
    blinkPhase = 0;
  }
  if (isBlinking) {
    blinkPhase += dt * 6.0f; // blink duration ~0.33s
    if (blinkPhase >= 2.0f) {
      isBlinking = false;
      blinkPhase = 0;
      blinkTimer = render.blinkRate + random(-100, 100) / 100.0f;
    }
  }
  // Blink factor with anticipation: slight squint before closing
  float bf = 0;
  if (isBlinking) {
    if (blinkPhase < 0.15f) {
      // Anticipation: slight squint
      bf = easeIn(blinkPhase / 0.15f) * 0.1f;
    } else if (blinkPhase < 1.0f) {
      // Close: fast ease-in
      bf = 0.1f + easeIn((blinkPhase - 0.15f) / 0.85f) * 0.9f;
    } else {
      // Open: slower ease-out (follow-through)
      bf = 1.0f - easeOut((blinkPhase - 1.0f));
    }
  }

  // --- Saccades (micro eye movements) ---
  saccadeTimer -= dt;
  if (saccadeTimer <= 0) {
    saccadeX = (random(-100, 100) / 100.0f) * render.saccadeAmount * 6.0f;
    saccadeY = (random(-100, 100) / 100.0f) * render.saccadeAmount * 4.0f;
    saccadeTimer = 0.08f + random(0, 200) / 1000.0f;
  }

  // --- Look-around idle animation ---
  // Periodically pick a dramatic gaze direction, hold it, then return
  lookAroundBaseX = render.gazeX;
  lookAroundBaseY = render.gazeY;

  if (isLookingAround) {
    lookAroundHoldTimer -= dt;
    if (lookAroundHoldTimer <= 0) {
      // Done looking — return to base gaze
      isLookingAround = false;
      gazeTargetX = lookAroundBaseX;
      gazeTargetY = lookAroundBaseY;
      lookAroundTimer = 3.0f + random(0, 5000) / 1000.0f; // 3-8s until next look
    }
  } else {
    lookAroundTimer -= dt;
    if (lookAroundTimer <= 0) {
      isLookingAround = true;
      // Pick a dramatic direction
      int dir = random(0, 6);
      switch (dir) {
        case 0: gazeTargetX = -0.7f; gazeTargetY =  0.0f; break; // look left
        case 1: gazeTargetX =  0.7f; gazeTargetY =  0.0f; break; // look right
        case 2: gazeTargetX =  0.0f; gazeTargetY = -0.5f; break; // look up
        case 3: gazeTargetX =  0.0f; gazeTargetY =  0.4f; break; // look down
        case 4: gazeTargetX = -0.5f; gazeTargetY = -0.3f; break; // upper left
        case 5: gazeTargetX =  0.5f; gazeTargetY = -0.3f; break; // upper right
      }
      lookAroundHoldTimer = 0.8f + random(0, 1200) / 1000.0f; // hold 0.8-2s
    } else {
      // Small idle drift between look-arounds
      gazeTimer -= dt;
      if (gazeTimer <= 0) {
        gazeTargetX = lookAroundBaseX + (random(-100, 100) / 100.0f) * 0.15f;
        gazeTargetY = lookAroundBaseY + (random(-100, 100) / 100.0f) * 0.1f;
        gazeTimer = 1.5f + random(0, 2000) / 1000.0f;
      }
    }
  }

  // Smooth gaze with overshoot (saccade dart)
  float gazeSpeed = render.moveSpeed * dt;
  float dx = gazeTargetX - gazeCurrentX;
  float dy = gazeTargetY - gazeCurrentY;
  float dist = sqrtf(dx * dx + dy * dy);
  if (dist > 0.05f) {
    // Fast dart with overshoot for dramatic looks
    float overshoot = isLookingAround ? 1.2f : 1.1f;
    gazeCurrentX += dx * gazeSpeed * overshoot;
    gazeCurrentY += dy * gazeSpeed * overshoot;
  } else {
    // Settle (spring damping)
    gazeCurrentX += (gazeTargetX - gazeCurrentX) * 0.2f;
    gazeCurrentY += (gazeTargetY - gazeCurrentY) * 0.2f;
  }

  // --- Breathing ---
  breathPhase += dt * 1.5f;
  float breathScale = 1.0f + sinf(breathPhase) * 0.015f;

  // --- Compute geometry for TWO EYES ---
  float gap = 12.0f;
  float baseEyeW = render.width * 0.42f * breathScale;
  float baseEyeH = render.height * 0.55f * render.openness * breathScale;
  baseEyeH *= (1.0f - bf);
  baseEyeH = max(3.0f, baseEyeH);

  // Subtle asymmetry: right eye ~3% smaller (natural imperfection)
  float asymmetry = 0.97f;
  float eyeWL = baseEyeW, eyeHL = baseEyeH;
  float eyeWR = baseEyeW * asymmetry, eyeHR = baseEyeH * asymmetry;

  // Store per-eye sizes for the loop
  float eyeWArr[2] = {eyeWL, eyeWR};
  float eyeHArr[2] = {eyeHL, eyeHR};

  float eyeW = baseEyeW; // for positioning
  float eyeH = baseEyeH;

  float roundR = render.roundness * min(eyeW, eyeH) * 0.5f;

  float lcx = cx - gap / 2 - eyeW / 2;
  float rcx = cx + gap / 2 + eyeW / 2;
  float ecy = cy;

  // Colors
  uint16_t eyeCol  = hsv565(render.hue, render.saturation, render.brightness);
  uint16_t bgCol   = hsv565(render.hue, render.saturation * 0.15f, 0.05f);
  uint16_t glowCol = hsv565(render.hue, render.saturation * 0.6f, render.brightness * 0.3f * render.glowIntensity);
  uint16_t pupilCol = bgCol;

  // --- RENDER ---
  fbFillScreen(bgCol);

  // Glow behind both eyes
  if (render.glowIntensity > 0.05f) {
    int16_t gr = (int16_t)(render.glowIntensity * 50 + eyeW * 0.5f);
    for (int16_t r = gr; r > 0; r -= 3) {
      float t = (float)r / (float)gr;
      uint16_t gc = lerpColor565(bgCol, glowCol, t * t);
      fbFillCircle(cx, cy, r, gc);
    }
  }

  // Draw each eye
  for (int side = 0; side < 2; side++) {
    float ecx = (side == 0) ? lcx : rcx;
    int mirror = (side == 0) ? 1 : -1;
    float eyeW = eyeWArr[side];
    float eyeH = eyeHArr[side];
    float roundR = render.roundness * min(eyeW, eyeH) * 0.5f;

    int16_t ex = (int16_t)(ecx - eyeW / 2);
    int16_t ey = (int16_t)(ecy - eyeH / 2);

    // Eye body
    fbFillRoundRect(ex, ey, (int16_t)eyeW, (int16_t)eyeH, (int16_t)roundR, eyeCol);

    // Pupil (skip if emoji replaces it)
    if (eyeH > 8 && !pupilReplaced) {
      float pupR = render.pupilSize * min(eyeW, eyeH) * 0.45f;
      pupR = max(4.0f, pupR);
      float maxGazeX = (eyeW * 0.5f - pupR - 3);
      float maxGazeY = (eyeH * 0.5f - pupR - 3);
      int16_t px = (int16_t)(ecx + (gazeCurrentX + saccadeX / 15.0f) * maxGazeX);
      int16_t py = (int16_t)(ecy + (gazeCurrentY + saccadeY / 15.0f) * maxGazeY);

      // Pupil body
      fbFillCircle(px, py, (int16_t)pupR, pupilCol);
      // Inner dark core
      fbFillCircle(px, py, max((int16_t)2, (int16_t)(pupR * 0.4f)), 0x0000);
      // Specular highlight — follows gaze direction (Pixar technique)
      // Highlight shifts opposite to gaze (light source stays fixed, eye moves)
      int16_t hlR = max((int16_t)2, (int16_t)(pupR * 0.28f));
      float hlOffX = -gazeCurrentX * pupR * 0.15f - pupR * 0.2f;
      float hlOffY = -gazeCurrentY * pupR * 0.15f - pupR * 0.25f;
      fbFillCircle(px + (int16_t)hlOffX, py + (int16_t)hlOffY, hlR, 0xFFFF);
      // Secondary smaller highlight (adds life)
      int16_t hl2R = max((int16_t)1, (int16_t)(pupR * 0.12f));
      fbFillCircle(px + (int16_t)(hlOffX * 0.3f) + (int16_t)(pupR * 0.15f),
                    py + (int16_t)(hlOffY * 0.3f) + (int16_t)(pupR * 0.2f),
                    hl2R, 0xFFFF);
    }

    // --- Eyelids (mirrored for left/right) ---

    // Top lid: angry (lidAngle negative) = inner corners come DOWN
    float topDroop = render.lidTop;
    if (topDroop > 0.01f || fabsf(render.lidAngle) > 0.01f) {
      int16_t droopPx = (int16_t)(topDroop * eyeH * 0.6f);
      float rawLidAnglePx = render.lidAngle * eyeW * 0.3f;
      // Inner edge toward nose, outer toward ear
      int16_t innerX = (side == 0) ? ex + (int16_t)eyeW + 6 : ex - 6;
      int16_t outerX = (side == 0) ? ex - 6 : ex + (int16_t)eyeW + 6;
      // Angry (negative angle) = inner drops MORE (higher Y)
      int16_t innerDroop = droopPx + (int16_t)(-rawLidAnglePx);
      int16_t outerDroop = droopPx + (int16_t)(rawLidAnglePx);

      // Lid as a quad covering top of eye
      fbFillTriangle(
        innerX, ey - 6,
        outerX, ey - 6,
        innerX, ey + max((int16_t)0, innerDroop),
        bgCol);
      fbFillTriangle(
        outerX, ey - 6,
        outerX, ey + max((int16_t)0, outerDroop),
        innerX, ey + max((int16_t)0, innerDroop),
        bgCol);
    }

    // Bottom lid (happy squint)
    if (render.lidBottom > 0.01f) {
      int16_t raisePx = (int16_t)(render.lidBottom * eyeH * 0.5f);
      int16_t eyeBot = ey + (int16_t)eyeH;
      fbFillTriangle(
        ex - 6, eyeBot + 6,
        ex + (int16_t)eyeW + 6, eyeBot + 6,
        (int16_t)ecx, eyeBot - raisePx,
        bgCol);
    }

    // --- Eyebrow (disabled for now) ---
  }

  // --- Emoji overlay effects ---
  // Pass eye geometry for positioning
  int16_t eLx = (int16_t)lcx, eRx = (int16_t)rcx;
  int16_t eY = (int16_t)ecy;
  // Approximate pupil positions (center of each eye + gaze offset)
  int16_t pLx = (int16_t)(lcx + gazeCurrentX * baseEyeW * 0.15f);
  int16_t pRx = (int16_t)(rcx + gazeCurrentX * baseEyeW * 0.15f);
  int16_t pY  = (int16_t)(ecy + gazeCurrentY * baseEyeH * 0.15f);
  emojiFx->update(dt, eLx, eRx, eY, (int16_t)baseEyeW, (int16_t)baseEyeH, pLx, pRx, pY);
}
