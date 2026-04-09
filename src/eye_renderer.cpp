#include "eye_renderer.h"
#include <math.h>

#define C_WHITE 0xFFFF

EyeRenderer::EyeRenderer(uint16_t *framebuffer, int16_t width, int16_t height)
  : fb(framebuffer), w(width), h(height),
    cx(width / 2), cy(height / 2),
    tweenProgress(1.0f), tweenSpeed(0.04f),
    blinkTimer(0), blinkState(0), blinking(false),
    saccadeX(0), saccadeY(0), saccadeTimer(0),
    bouncePhase(0), lastFrameMs(0) {
  memset(&current, 0, sizeof(MoodParams));
  memset(&target, 0, sizeof(MoodParams));
}

// --- Drawing primitives (direct to framebuffer) ---

void EyeRenderer::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x >= 0 && x < w && y >= 0 && y < h) {
    fb[y * w + x] = color;
  }
}

void EyeRenderer::fillRect(int16_t x, int16_t y, int16_t rw, int16_t rh, uint16_t color) {
  int16_t x1 = max((int16_t)0, x);
  int16_t y1 = max((int16_t)0, y);
  int16_t x2 = min(w, (int16_t)(x + rw));
  int16_t y2 = min(h, (int16_t)(y + rh));
  for (int16_t j = y1; j < y2; j++) {
    for (int16_t i = x1; i < x2; i++) {
      fb[j * w + i] = color;
    }
  }
}

void EyeRenderer::fillScreen(uint16_t color) {
  for (int i = 0; i < w * h; i++) {
    fb[i] = color;
  }
}

void EyeRenderer::fillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color) {
  for (int16_t y = -r; y <= r; y++) {
    int16_t halfW = (int16_t)sqrtf(r * r - y * y);
    int16_t py = cy + y;
    if (py < 0 || py >= h) continue;
    int16_t x1 = max((int16_t)0, (int16_t)(cx - halfW));
    int16_t x2 = min(w, (int16_t)(cx + halfW + 1));
    for (int16_t x = x1; x < x2; x++) {
      fb[py * w + x] = color;
    }
  }
}

void EyeRenderer::fillRoundRect(int16_t x, int16_t y, int16_t rw, int16_t rh, int16_t r, uint16_t color) {
  r = min(r, (int16_t)(min(rw, rh) / 2));
  // Center rectangle
  fillRect(x + r, y, rw - 2 * r, rh, color);
  // Left and right rectangles
  fillRect(x, y + r, r, rh - 2 * r, color);
  fillRect(x + rw - r, y + r, r, rh - 2 * r, color);
  // Four corner circles
  fillCircle(x + r, y + r, r, color);
  fillCircle(x + rw - r - 1, y + r, r, color);
  fillCircle(x + r, y + rh - r - 1, r, color);
  fillCircle(x + rw - r - 1, y + rh - r - 1, r, color);
}

void EyeRenderer::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  // Sort vertices by y
  if (y0 > y1) { int16_t t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }
  if (y1 > y2) { int16_t t; t=x1;x1=x2;x2=t; t=y1;y1=y2;y2=t; }
  if (y0 > y1) { int16_t t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }

  for (int16_t y = max((int16_t)0, y0); y <= min((int16_t)(h - 1), y2); y++) {
    float xa, xb;
    if (y < y1) {
      if (y1 == y0) xa = x0; else xa = x0 + (float)(x1 - x0) * (y - y0) / (y1 - y0);
      if (y2 == y0) xb = x0; else xb = x0 + (float)(x2 - x0) * (y - y0) / (y2 - y0);
    } else {
      if (y2 == y1) xa = x1; else xa = x1 + (float)(x2 - x1) * (y - y1) / (y2 - y1);
      if (y2 == y0) xb = x0; else xb = x0 + (float)(x2 - x0) * (y - y0) / (y2 - y0);
    }
    if (xa > xb) { float t = xa; xa = xb; xb = t; }
    int16_t ix1 = max((int16_t)0, (int16_t)xa);
    int16_t ix2 = min(w, (int16_t)(xb + 1));
    for (int16_t x = ix1; x < ix2; x++) {
      fb[y * w + x] = color;
    }
  }
}

// --- Interpolation ---

uint8_t EyeRenderer::lerpU8(uint8_t a, uint8_t b, float t) {
  return (uint8_t)(a + (b - a) * t);
}

int8_t EyeRenderer::lerpI8(int8_t a, int8_t b, float t) {
  return (int8_t)(a + (b - a) * t);
}

uint16_t EyeRenderer::lerpColor(uint16_t a, uint16_t b, float t) {
  uint8_t r1 = (a >> 11) & 0x1F, g1 = (a >> 5) & 0x3F, b1 = a & 0x1F;
  uint8_t r2 = (b >> 11) & 0x1F, g2 = (b >> 5) & 0x3F, b2 = b & 0x1F;
  uint8_t r = r1 + (int)(r2 - r1) * t;
  uint8_t g = g1 + (int)(g2 - g1) * t;
  uint8_t bl = b1 + (int)(b2 - b1) * t;
  return (r << 11) | (g << 5) | bl;
}

void EyeRenderer::lerpMood(MoodParams &out, const MoodParams &a, const MoodParams &b, float t) {
  out.eye_width      = lerpU8(a.eye_width, b.eye_width, t);
  out.eye_height     = lerpU8(a.eye_height, b.eye_height, t);
  out.border_radius  = lerpU8(a.border_radius, b.border_radius, t);
  out.openness       = lerpU8(a.openness, b.openness, t);
  out.pupil_size     = lerpU8(a.pupil_size, b.pupil_size, t);
  out.pupil_x        = lerpI8(a.pupil_x, b.pupil_x, t);
  out.pupil_y        = lerpI8(a.pupil_y, b.pupil_y, t);
  out.lid_top        = lerpU8(a.lid_top, b.lid_top, t);
  out.lid_bottom     = lerpU8(a.lid_bottom, b.lid_bottom, t);
  out.lid_angle      = lerpI8(a.lid_angle, b.lid_angle, t);
  out.eye_color      = lerpColor(a.eye_color, b.eye_color, t);
  out.bg_color       = lerpColor(a.bg_color, b.bg_color, t);
  out.glow_color     = lerpColor(a.glow_color, b.glow_color, t);
  out.glow_radius    = lerpU8(a.glow_radius, b.glow_radius, t);
  out.blink_interval = lerpU8(a.blink_interval, b.blink_interval, t);
  out.blink_variation= lerpU8(a.blink_variation, b.blink_variation, t);
  out.saccade_speed  = lerpU8(a.saccade_speed, b.saccade_speed, t);
  out.idle_drift     = lerpU8(a.idle_drift, b.idle_drift, t);
  out.bounce_amp     = lerpU8(a.bounce_amp, b.bounce_amp, t);
  out.bounce_speed   = lerpU8(a.bounce_speed, b.bounce_speed, t);
}

// --- Public API ---

void EyeRenderer::setMood(const MoodParams &mood) {
  // Start tweening from current rendered state to new target
  target = mood;
  tweenProgress = 0.0f;
}

void EyeRenderer::setMoodImmediate(const MoodParams &mood) {
  current = mood;
  target = mood;
  tweenProgress = 1.0f;
}

void EyeRenderer::blink() {
  blinking = true;
  blinkState = 0.0f;
}

// --- Main update/render ---

void EyeRenderer::update() {
  unsigned long now = millis();
  float dt = (lastFrameMs == 0) ? 0.033f : (now - lastFrameMs) / 1000.0f;
  lastFrameMs = now;
  dt = min(dt, 0.1f); // clamp

  // --- Tweening ---
  if (tweenProgress < 1.0f) {
    tweenProgress += tweenSpeed;
    if (tweenProgress >= 1.0f) {
      tweenProgress = 1.0f;
      current = target;
    }
  }

  // Compute interpolated params for this frame
  MoodParams render;
  if (tweenProgress < 1.0f) {
    lerpMood(render, current, target, tweenProgress);
  } else {
    render = current;
  }

  // --- Blink cycle ---
  blinkTimer -= dt;
  if (blinkTimer <= 0 && !blinking) {
    blinking = true;
    blinkState = 0.0f;
  }
  if (blinking) {
    blinkState += dt * 8.0f; // blink speed
    if (blinkState >= 2.0f) {
      // Blink complete (0→1→0 over blinkState 0→2)
      blinking = false;
      blinkState = 0.0f;
      float interval = render.blink_interval + random(0, render.blink_variation + 1);
      blinkTimer = max(1.0f, interval);
    }
  }
  // blinkFactor: 0=open, 1=closed
  float blinkFactor = 0;
  if (blinking) {
    blinkFactor = (blinkState < 1.0f) ? blinkState : (2.0f - blinkState);
  }

  // --- Saccade (micro eye movements) ---
  saccadeTimer -= dt;
  if (saccadeTimer <= 0) {
    float speed = render.saccade_speed / 10.0f;
    saccadeX = (random(-100, 100) / 100.0f) * speed * 8.0f;
    saccadeY = (random(-100, 100) / 100.0f) * speed * 5.0f;
    saccadeTimer = 0.1f + random(0, 300) / 1000.0f;
  }

  // --- Bounce/breathing ---
  bouncePhase += dt * render.bounce_speed * 0.5f;
  float bounceOffset = sinf(bouncePhase) * render.bounce_amp * 0.5f;

  // --- Compute final eye geometry ---
  float eyeW = render.eye_width;
  float eyeH = render.eye_height * render.openness / 100.0f;
  // Apply blink
  eyeH *= (1.0f - blinkFactor);
  eyeH = max(2.0f, eyeH);
  // Apply bounce
  eyeH += bounceOffset;

  int16_t ex = cx - (int16_t)(eyeW / 2);
  int16_t ey = cy - (int16_t)(eyeH / 2);

  // --- RENDER ---

  // Background
  fillScreen(render.bg_color);

  // Glow behind eye
  if (render.glow_radius > 0) {
    int16_t gr = (int16_t)render.glow_radius + (int16_t)(eyeW / 3);
    // Gradient glow: concentric circles getting dimmer
    for (int16_t r = gr; r > 0; r -= 3) {
      float t = (float)r / (float)gr;
      uint16_t glowCol = lerpColor(render.bg_color, render.glow_color, t * 0.4f);
      fillCircle(cx, cy, r, glowCol);
    }
  }

  // Eye body (rounded rectangle)
  int16_t br = min((int16_t)render.border_radius, (int16_t)(min(eyeW, eyeH) / 2));
  fillRoundRect(ex, ey, (int16_t)eyeW, (int16_t)eyeH, br, render.eye_color);

  // Pupil
  if (eyeH > 10) {
    int16_t ps = (int16_t)(render.pupil_size * (eyeH / render.eye_height));
    ps = max((int16_t)5, ps);
    int16_t px = cx + render.pupil_x + (int16_t)saccadeX;
    int16_t py = cy + render.pupil_y + (int16_t)saccadeY;
    // Constrain pupil inside eye
    px = constrain(px, ex + ps + 2, ex + (int16_t)eyeW - ps - 2);
    py = constrain(py, ey + ps + 2, ey + (int16_t)eyeH - ps - 2);
    fillCircle(px, py, ps, render.bg_color);
    // Inner pupil dot
    fillCircle(px, py, max(3, ps / 3), 0x0000);
    // Specular highlight
    fillCircle(px - ps / 3, py - ps / 3, max(2, ps / 5), C_WHITE);
  }

  // --- Eyelids ---
  uint16_t lidColor = render.bg_color;

  // Top eyelid
  if (render.lid_top > 0 || render.lid_angle != 0) {
    int16_t droop = (int16_t)(render.lid_top * eyeH / 100.0f);
    int16_t angleOffset = (int16_t)(render.lid_angle * eyeW / 200.0f);
    // Triangle overlay from top of eye
    fillTriangle(
      ex - 5, ey - 5,
      ex + (int16_t)eyeW + 5, ey - 5,
      ex - 5 - angleOffset, ey + droop,
      lidColor
    );
    fillTriangle(
      ex + (int16_t)eyeW + 5, ey - 5,
      ex + (int16_t)eyeW + 5 - angleOffset, ey + droop,
      ex - 5 - angleOffset, ey + droop,
      lidColor
    );
  }

  // Bottom eyelid (for happy squint)
  if (render.lid_bottom > 0) {
    int16_t raise = (int16_t)(render.lid_bottom * eyeH / 100.0f);
    int16_t eyeBottom = ey + (int16_t)eyeH;
    fillTriangle(
      ex - 5, eyeBottom + 5,
      ex + (int16_t)eyeW + 5, eyeBottom + 5,
      ex - 5, eyeBottom - raise,
      lidColor
    );
    fillTriangle(
      ex + (int16_t)eyeW + 5, eyeBottom + 5,
      ex + (int16_t)eyeW + 5, eyeBottom - raise,
      ex - 5, eyeBottom - raise,
      lidColor
    );
  }
}
