#include <Arduino.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "CinzelDecorative16pt.h"

// XIAO ESP32-C6 -> GC9A01 (corrected pin mapping)
#define TFT_DC    0   // D0 = GPIO 0
#define TFT_CS    1   // D1 = GPIO 1
#define TFT_RST   2   // D2 = GPIO 2
#define TFT_SCK  19   // D8 = GPIO 19
#define TFT_MOSI 18   // D10 = GPIO 18

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true, 240, 240, 0, 0, 0, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(240, 240, gfx);

#define CX 120
#define CY 120
#define BALL_R 115

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

struct Particle {
  float angle, radius, speed, wobble;
  uint8_t brightness;
};

#define NUM_PARTICLES 40
Particle particles[NUM_PARTICLES];
float phase = 0;
float glowPulse = 0;

// Affirmations (shorter to fit the ornate font)
const char *affirmations[] = {
  "You are\nenough",
  "Trust the\nprocess",
  "You are\ngrowing",
  "Breathe",
  "Be kind\nto yourself",
  "You are\nloved",
  "Let go",
  "You are\nmagic",
  "Believe",
  "Peace\nwithin",
  "You\nshine",
  "Radiant",
  "Stars\nalign",
  "You are\nwhole",
  "Embrace\nthe now",
};
#define NUM_AFFIRMATIONS (sizeof(affirmations) / sizeof(affirmations[0]))

unsigned long cycleStart = 0;
bool wasShowingText = false;
int currentAffirmation = 0;

// Cycle: 10s animation, 5s text (with 1s fade in, 3s hold, 1s fade out)
#define ANIM_MS    10000
#define TEXT_MS     5000
#define FADE_MS     1000
#define CYCLE_MS   (ANIM_MS + TEXT_MS)

void initParticles() {
  for (int i = 0; i < NUM_PARTICLES; i++) {
    particles[i].angle = random(0, 3600) / 10.0f;
    particles[i].radius = random(8, 85);
    particles[i].speed = (random(5, 25) / 1000.0f) * (random(0, 2) ? 1 : -1);
    particles[i].wobble = random(1, 6) / 10.0f;
    particles[i].brightness = random(100, 255);
  }
}

void drawBallBackground() {
  canvas->fillScreen(0x0000);
  canvas->fillCircle(CX, CY, BALL_R, rgb565(15, 8, 50));

  // Pulsing core glow
  float pulse = (sin(glowPulse) + 1.0f) * 0.5f;
  int glowR = 30 + (int)(25 * pulse);
  uint8_t gi = (uint8_t)(40 + 80 * pulse);
  canvas->fillCircle(CX, CY, glowR, rgb565(gi / 3, gi / 5, gi));

  // Swirling particles
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Particle &p = particles[i];
    p.angle += p.speed;
    p.radius += sin(phase + p.angle) * p.wobble * 0.3f;
    p.radius = constrain(p.radius, 5, 95);

    float rad = p.angle * DEG_TO_RAD;
    int px = CX + (int)(p.radius * cos(rad));
    int py = CY + (int)(p.radius * sin(rad));

    float dx = px - CX;
    float dy = py - CY;
    if (dx * dx + dy * dy < BALL_R * BALL_R) {
      uint8_t bri = (uint8_t)(p.brightness * (0.4f + 0.6f * sin(phase * 3.0f + i * 0.7f)));
      uint16_t col = rgb565((uint8_t)(bri * 0.55f), (uint8_t)(bri * 0.35f), bri);

      if (bri > 150) {
        canvas->fillCircle(px, py, 2, col);
      } else {
        canvas->drawPixel(px, py, col);
      }
    }
  }



  phase += 0.03f;
  glowPulse += 0.06f;
}

void drawTextOverlay(float alpha) {
  // alpha 0.0 = invisible, 1.0 = fully visible
  if (alpha <= 0.01f) return;

  uint8_t a8 = (uint8_t)(alpha * 255);
  uint16_t textCol = rgb565(
    (uint8_t)(220 * a8 / 255),
    (uint8_t)(200 * a8 / 255),
    (uint8_t)(255 * a8 / 255)
  );

  canvas->setFont(&CinzelDecorative_Bold16pt7b);
  canvas->setTextColor(textCol);
  canvas->setTextWrap(false);

  const char *text = affirmations[currentAffirmation];

  // Count lines
  int lines = 1;
  for (const char *p = text; *p; p++) {
    if (*p == '\n') lines++;
  }
  int lineH = 28;
  int startY = CY - (lines * lineH) / 2 + lineH / 2;

  // Draw each line centered
  char buf[64];
  strncpy(buf, text, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';
  char *line = strtok(buf, "\n");
  int y = startY;
  while (line) {
    int16_t x1, y1;
    uint16_t w, h;
    canvas->getTextBounds(line, 0, 0, &x1, &y1, &w, &h);
    canvas->setCursor(CX - w / 2 - x1, y);
    canvas->print(line);
    y += lineH;
    line = strtok(NULL, "\n");
  }

  canvas->setFont(NULL); // Reset to default
}

void setup() {
  Serial.begin(115200);
  delay(500);

  gfx->begin();
  gfx->fillScreen(0x0000);
  delay(100);
  gfx->fillScreen(0x0000);
  canvas->begin();
  // Extra: clear canvas once and flush to ensure GRAM is black
  canvas->fillScreen(0x0000);
  canvas->flush();
  initParticles();
  cycleStart = millis();
  Serial.println("Crystal Ball ready");
}

void loop() {
  unsigned long elapsed = (millis() - cycleStart) % CYCLE_MS;
  bool inTextPhase = elapsed >= ANIM_MS;

  // Pick new affirmation at start of text phase
  if (inTextPhase && !wasShowingText) {
    currentAffirmation = random(0, NUM_AFFIRMATIONS);
    wasShowingText = true;
  } else if (!inTextPhase) {
    wasShowingText = false;
  }

  // Always draw the ball animation
  drawBallBackground();

  // Overlay text with fade
  if (inTextPhase) {
    unsigned long textElapsed = elapsed - ANIM_MS;
    float alpha;
    if (textElapsed < FADE_MS) {
      alpha = (float)textElapsed / FADE_MS;           // Fade in
    } else if (textElapsed > TEXT_MS - FADE_MS) {
      alpha = (float)(TEXT_MS - textElapsed) / FADE_MS; // Fade out
    } else {
      alpha = 1.0f;                                     // Full
    }
    drawTextOverlay(alpha);
  }

  canvas->flush();
}
