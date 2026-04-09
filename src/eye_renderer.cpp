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

  // EYE SIZE: arousal drives size (alert=big, calm=smaller)
  //           pleasure adds slight enlargement (happy=bigger)
  float baseSize = 130.0f;
  out.width  = baseSize + a * 25.0f + p * 10.0f;
  out.height = baseSize + a * 30.0f + p * 15.0f;
  // Dominance makes eye slightly wider (confident = wider gaze)
  out.width += d * 10.0f;

  // ROUNDNESS: pleasure = rounder, anger = more rectangular
  out.roundness = 0.6f + p * 0.3f - (a > 0 && p < 0 ? 0.2f : 0.0f);
  out.roundness = constrain(out.roundness, 0.2f, 1.0f);

  // OPENNESS: arousal = wider open, low arousal = squinting/sleepy
  out.openness = 0.7f + a * 0.25f;
  if (a < -0.5f) out.openness -= 0.15f; // extra droop when very calm/sleepy
  out.openness = constrain(out.openness, 0.15f, 1.0f);

  // PUPIL: arousal dilates pupil (fight/flight), pleasure slightly too
  out.pupilSize = 0.35f + a * 0.15f + p * 0.05f;
  out.pupilSize = constrain(out.pupilSize, 0.15f, 0.6f);

  // GAZE: dominance affects gaze (dominant = direct, submissive = averted/down)
  out.gazeX = 0;
  out.gazeY = d * -0.15f; // dominant looks slightly up, submissive looks down
  if (d < -0.3f) out.gazeX = -0.2f; // submissive looks slightly away

  // LID TOP: tired (low arousal, low pleasure) = droopy top lid
  //          angry (high arousal, low pleasure) = angled down lid
  out.lidTop = 0.0f;
  if (a < 0) out.lidTop = fabsf(a) * 0.4f;  // sleepy/calm = droop
  if (p < -0.3f && a > 0.3f) out.lidTop = 0.2f; // angry = partial squint

  // LID BOTTOM: happy = raised bottom (squinty smile), surprised = none
  out.lidBottom = 0.0f;
  if (p > 0.3f) out.lidBottom = p * 0.35f;  // happy squint
  if (a > 0.7f && p >= 0) out.lidBottom = 0.0f; // surprised overrides squint

  // LID ANGLE: angry = inner corners up (-), tired = outer up (+)
  out.lidAngle = 0.0f;
  if (p < -0.3f && a > 0.3f) out.lidAngle = -0.5f - p * 0.3f;  // angry
  if (p < 0 && a < -0.3f)    out.lidAngle = 0.3f + fabsf(a) * 0.2f; // tired/sad
  if (d > 0.5f && p < 0)     out.lidAngle -= 0.2f; // dominant + unhappy = more angry
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
    tweenT(1.0f), lastMs(0),
    blinkTimer(3.0f), blinkPhase(0), isBlinking(false),
    saccadeX(0), saccadeY(0), saccadeTimer(0),
    breathPhase(0),
    gazeTargetX(0), gazeTargetY(0),
    gazeCurrentX(0), gazeCurrentY(0), gazeTimer(0) {
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

void EyeRenderer::setMood(const Mood &mood) {
  setEmotion(padToFloat(mood.pleasure), padToFloat(mood.arousal), padToFloat(mood.dominance));
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

  // --- Idle gaze drift ---
  gazeTimer -= dt;
  if (gazeTimer <= 0) {
    gazeTargetX = render.gazeX + (random(-100, 100) / 100.0f) * render.saccadeAmount * 0.3f;
    gazeTargetY = render.gazeY + (random(-100, 100) / 100.0f) * render.saccadeAmount * 0.2f;
    gazeTimer = 1.0f + random(0, 2000) / 1000.0f;
  }
  // Smooth gaze movement
  float gazeSpeed = render.moveSpeed * dt;
  gazeCurrentX += (gazeTargetX - gazeCurrentX) * gazeSpeed;
  gazeCurrentY += (gazeTargetY - gazeCurrentY) * gazeSpeed;

  // --- Breathing ---
  breathPhase += dt * 1.5f;
  float breathScale = 1.0f + sinf(breathPhase) * 0.015f;

  // --- Compute geometry for TWO EYES ---
  float gap = 12.0f; // space between eyes
  float eyeW = render.width * 0.42f * breathScale; // each eye is ~42% of mood width
  float eyeH = render.height * 0.55f * render.openness * breathScale;
  eyeH *= (1.0f - bf); // blink closes eyes
  eyeH = max(3.0f, eyeH);

  float roundR = render.roundness * min(eyeW, eyeH) * 0.5f;

  // Left eye center, right eye center
  float lcx = cx - gap / 2 - eyeW / 2;
  float rcx = cx + gap / 2 + eyeW / 2;
  float ecy = cy; // vertical center

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
    int mirror = (side == 0) ? 1 : -1; // mirror lid angles for right eye

    int16_t ex = (int16_t)(ecx - eyeW / 2);
    int16_t ey = (int16_t)(ecy - eyeH / 2);

    // Eye body
    fbFillRoundRect(ex, ey, (int16_t)eyeW, (int16_t)eyeH, (int16_t)roundR, eyeCol);

    // Pupil
    if (eyeH > 8) {
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
      // Specular highlight
      int16_t hlR = max((int16_t)1, (int16_t)(pupR * 0.22f));
      fbFillCircle(px - (int16_t)(pupR * 0.25f), py - (int16_t)(pupR * 0.25f), hlR, 0xFFFF);
    }

    // --- Eyelids (mirrored for left/right) ---

    // Top lid: angry = inner corners down (angle mirrored per eye)
    float topDroop = render.lidTop;
    float angleOff = render.lidAngle * mirror;
    if (topDroop > 0.01f || fabsf(angleOff) > 0.01f) {
      int16_t droopPx = (int16_t)(topDroop * eyeH * 0.6f);
      int16_t anglePx = (int16_t)(angleOff * eyeW * 0.3f);
      fbFillTriangle(
        ex - 6, ey - 6,
        ex + (int16_t)eyeW + 6, ey - 6,
        ex - 6 + anglePx, ey + droopPx,
        bgCol);
      fbFillTriangle(
        ex + (int16_t)eyeW + 6, ey - 6,
        ex + (int16_t)eyeW + 6 - anglePx, ey + droopPx,
        ex - 6 + anglePx, ey + droopPx,
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

    // --- Eyebrow (curved, can cut into eye) ---
    {
      float browW = eyeW * 1.1f;
      float browH = 6.0f + render.browThickness * 10.0f;
      float browBaseY = ey - 4.0f - render.browHeight * eyeH * 0.4f;
      float rawAnglePx = render.browAngle * eyeW * 0.2f;
      float curveAmt = render.browCurve * eyeH * 0.25f; // max curve displacement

      uint16_t browCol = hsv565(render.hue, render.saturation * 0.4f, render.brightness * 0.35f);

      int16_t bx = (int16_t)(ecx - browW / 2);
      int steps = (int)browW;

      for (int i = 0; i < steps; i++) {
        float t = (float)i / (float)(steps - 1); // 0..1 across brow width

        // X position
        int16_t sx = bx + i;

        // Angle: linear interpolation from inner to outer
        // Inner is the side closer to nose
        float angleT = (side == 0) ? (1.0f - t) : t; // 0=outer, 1=inner for both eyes
        float angleY = rawAnglePx * (1.0f - 2.0f * angleT); // inner goes opposite

        // Curve: parabola peaking at center of brow (t=0.5)
        // Positive curve = arch up (lower Y), negative = sag down (higher Y)
        float curveY = -curveAmt * 4.0f * (t - 0.5f) * (t - 0.5f) + curveAmt;
        // curveY is max at center, 0 at edges

        float finalY = browBaseY + angleY - curveY;

        // Draw a vertical strip
        fbFillRect(sx, (int16_t)finalY, 1, (int16_t)browH, browCol);
      }
    }
  }
}
