#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <ArduinoOTA.h>
#include <Fonts/FreeSans9pt7b.h>
#include "moods.h"
#include "eye_renderer.h"
#include "mqtt_handler.h"
#include "needs.h"

// XIAO ESP32-C6 -> GC9A01
#define TFT_DC    0
#define TFT_CS    1
#define TFT_RST   2

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 *canvas = NULL;
uint16_t *framebuffer = NULL;
EyeRenderer *eye = NULL;
MqttHandler mqtt;
NeedsEngine needs;

// Current mood tracking
int currentMoodIndex = 0;
const char *currentMoodName = "neutral";
int lastNeedsMoodIdx = -1;

// Power state
bool screenOn = true;
bool powerAnimating = false;

// Now Playing overlay state
#define OVL_FADE_MS 500
unsigned long overlayStartMs = 0;
uint32_t overlayHoldMs = 0; // 0 = inactive
char overlayTitle[96] = "";
char overlayArtist[96] = "";

uint8_t overlayFaceAlpha(unsigned long now) {
  if (overlayHoldMs == 0) return 255;
  unsigned long elapsed = now - overlayStartMs;
  uint32_t fadeOutEnd = OVL_FADE_MS;
  uint32_t holdEnd    = fadeOutEnd + overlayHoldMs;
  uint32_t fadeInEnd  = holdEnd + OVL_FADE_MS;
  if (elapsed >= fadeInEnd) { overlayHoldMs = 0; return 255; }
  if (elapsed < fadeOutEnd) return 255 - (elapsed * 255 / OVL_FADE_MS);
  if (elapsed < holdEnd)    return 0;
  return (elapsed - holdEnd) * 255 / OVL_FADE_MS;
}

void startNowPlayingOverlay(const char *title, const char *artist, uint32_t holdMs) {
  if (!screenOn) return; // music shouldn't wake the dweller
  if (!title || !*title) return;
  strncpy(overlayTitle,  title  ? title  : "", sizeof(overlayTitle)  - 1);
  strncpy(overlayArtist, artist ? artist : "", sizeof(overlayArtist) - 1);
  overlayTitle[sizeof(overlayTitle) - 1] = 0;
  overlayArtist[sizeof(overlayArtist) - 1] = 0;

  // If face is already hidden (we're in hold or fading in), jump to hold start
  // with the new track instead of double-fading.
  unsigned long now = millis();
  bool faceHidden = (overlayHoldMs > 0) && (now - overlayStartMs) >= OVL_FADE_MS;
  overlayHoldMs = holdMs > 0 ? holdMs : 10000;
  overlayStartMs = faceHidden ? (now - OVL_FADE_MS) : now;
  Serial.printf("NowPlaying: %s — %s (%lums)\n", overlayTitle, overlayArtist, overlayHoldMs);
}

// Dim every pixel in the framebuffer by `factor` (0=black, 255=unchanged).
void dimFramebuffer(uint8_t factor) {
  if (factor == 255) return;
  uint16_t *p = framebuffer;
  const int N = 240 * 240;
  if (factor == 0) {
    memset(p, 0, N * sizeof(uint16_t));
    return;
  }
  for (int i = 0; i < N; i++) {
    uint16_t c = p[i];
    if (c == 0) continue;
    uint8_t r = (c >> 11) & 0x1F;
    uint8_t g = (c >> 5)  & 0x3F;
    uint8_t b =  c        & 0x1F;
    r = (r * factor) >> 8;
    g = (g * factor) >> 8;
    b = (b * factor) >> 8;
    p[i] = (r << 11) | (g << 5) | b;
  }
}

// Draw a single line of text centered at y, with given color, truncating with
// "..." if it exceeds maxWidth.
static void drawCenteredLine(const char *text, int16_t y, uint16_t color, int16_t maxWidth) {
  if (!text || !*text) return;
  char buf[96];
  strncpy(buf, text, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = 0;

  int16_t x1, y1; uint16_t tw, th;
  canvas->getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
  if (tw > maxWidth) {
    while (tw > maxWidth && strlen(buf) > 4) {
      buf[strlen(buf) - 1] = 0;
      canvas->getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
    }
    size_t L = strlen(buf);
    if (L >= 3) { buf[L-3] = '.'; buf[L-2] = '.'; buf[L-1] = '.'; }
    canvas->getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
  }
  int16_t tx = (240 - (int16_t)tw) / 2 - x1;
  canvas->setCursor(tx, y);
  canvas->setTextColor(color);
  canvas->print(buf);
}

void renderNowPlayingText(uint8_t alpha) {
  if (alpha == 0) return;
  // Title: FreeSans 9pt white, ~50px wide safe margin from edges of round screen
  uint16_t titleColor  = RGB565(alpha, alpha, alpha);
  // Artist: dimmer (~60% of title), slight cool tint
  uint8_t aa = (alpha * 153) >> 8;
  uint16_t artistColor = RGB565(aa, aa, (uint8_t)min(255, aa + 30));

  canvas->setFont(&FreeSans9pt7b);
  canvas->setTextSize(1);
  canvas->setTextWrap(false);
  drawCenteredLine(overlayTitle, 115, titleColor, 200);
  if (overlayArtist[0]) {
    canvas->setFont(NULL); // default 5x8 for artist
    canvas->setTextSize(1);
    drawCenteredLine(overlayArtist, 140, artistColor, 220);
  }
  canvas->setFont(NULL);
}

void crtOff() {
  if (!screenOn || powerAnimating) return;
  powerAnimating = true;
  screenOn = false;

  // Capture current frame as starting point
  // Phase 1: Squeeze vertical to a horizontal line (center)
  for (int step = 0; step < 20; step++) {
    float t = (float)step / 19.0f;
    float squeeze = 1.0f - t; // 1.0 → 0.0
    int16_t halfH = max((int16_t)1, (int16_t)(120.0f * squeeze));
    int16_t top = 120 - halfH;
    int16_t bot = 120 + halfH;

    // Black out above and below the shrinking band
    for (int y = 0; y < top; y++)
      for (int x = 0; x < 240; x++)
        framebuffer[y * 240 + x] = 0;
    for (int y = bot; y < 240; y++)
      for (int x = 0; x < 240; x++)
        framebuffer[y * 240 + x] = 0;

    // Brighten the remaining line
    if (halfH <= 3) {
      for (int y = top; y < bot; y++)
        for (int x = 0; x < 240; x++)
          framebuffer[y * 240 + x] = 0xFFFF;
    }

    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(15);
  }

  // Phase 2: Horizontal line shrinks to a dot
  for (int step = 0; step < 15; step++) {
    float t = (float)step / 14.0f;
    int16_t halfW = max((int16_t)1, (int16_t)(120.0f * (1.0f - t)));
    memset(framebuffer, 0, 240 * 240 * 2);
    // Bright dot/line at center
    int16_t dotH = max((int)1, 3 - step / 5);
    for (int y = 120 - dotH; y <= 120 + dotH; y++)
      for (int x = 120 - halfW; x <= 120 + halfW; x++)
        if (x >= 0 && x < 240 && y >= 0 && y < 240)
          framebuffer[y * 240 + x] = 0xFFFF;

    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(20);
  }

  // Phase 3: Dot fades out
  for (int step = 0; step < 8; step++) {
    memset(framebuffer, 0, 240 * 240 * 2);
    uint8_t bri = 255 - step * 32;
    uint16_t col = RGB565(bri, bri, bri);
    int16_t r = max((int)1, 3 - step / 3);
    for (int y = 120 - r; y <= 120 + r; y++)
      for (int x = 120 - r; x <= 120 + r; x++)
        if (x >= 0 && x < 240 && y >= 0 && y < 240)
          framebuffer[y * 240 + x] = col;
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(30);
  }

  // Full black
  memset(framebuffer, 0, 240 * 240 * 2);
  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);

  powerAnimating = false;
  Serial.println("Power: OFF (CRT shutdown)");
}

void crtOn() {
  if (screenOn || powerAnimating) return;
  powerAnimating = true;

  // Phase 1: Bright dot appears at center
  for (int step = 0; step < 8; step++) {
    memset(framebuffer, 0, 240 * 240 * 2);
    uint8_t bri = step * 32;
    uint16_t col = RGB565(bri, bri, bri);
    int16_t r = max(1, step / 3);
    for (int y = 120 - r; y <= 120 + r; y++)
      for (int x = 120 - r; x <= 120 + r; x++)
        if (x >= 0 && x < 240 && y >= 0 && y < 240)
          framebuffer[y * 240 + x] = col;
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(30);
  }

  // Phase 2: Dot stretches to horizontal line
  for (int step = 0; step < 15; step++) {
    float t = (float)step / 14.0f;
    int16_t halfW = max((int16_t)1, (int16_t)(120.0f * t));
    memset(framebuffer, 0, 240 * 240 * 2);
    int16_t dotH = max(1, min(3, step / 4));
    for (int y = 120 - dotH; y <= 120 + dotH; y++)
      for (int x = 120 - halfW; x <= 120 + halfW; x++)
        if (x >= 0 && x < 240 && y >= 0 && y < 240)
          framebuffer[y * 240 + x] = 0xFFFF;
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(20);
  }

  // Phase 3: Line expands vertically to full screen (with slight static)
  for (int step = 0; step < 20; step++) {
    float t = (float)step / 19.0f;
    int16_t halfH = max((int16_t)1, (int16_t)(120.0f * t));
    memset(framebuffer, 0, 240 * 240 * 2);
    // Fill expanding area with slight blue-white static
    for (int y = 120 - halfH; y < 120 + halfH; y++)
      for (int x = 0; x < 240; x++) {
        uint8_t noise = random(180, 255);
        framebuffer[y * 240 + x] = RGB565(noise, noise, (uint8_t)min(255, noise + 20));
      }
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
    delay(15);
  }

  // Phase 4: Static fades to normal render
  screenOn = true;
  powerAnimating = false;
  Serial.println("Power: ON (CRT boot)");
}

// Handle face commands from MQTT
void onFaceCommand(const FaceCommand &cmd) {
  needs.onMqttCommand();
  // Wake up if screen is off
  if (!screenOn) crtOn();

  if (cmd.hasMoodName && cmd.moodIndex >= 0) {
    // Preset mood
    currentMoodIndex = cmd.moodIndex;
    currentMoodName = MOODS[cmd.moodIndex].name;
    eye->setMood(MOODS[cmd.moodIndex], cmd.moodIndex);
    Serial.printf("MQTT: mood preset -> %s\n", currentMoodName);
  }
  else if (cmd.hasPAD) {
    float p = cmd.p / 100.0f, a = cmd.a / 100.0f, d = cmd.d / 100.0f;
    currentMoodName = "mqtt_custom";
    currentMoodIndex = -1;

    if (cmd.hasEmoji) {
      eye->setFace(p, a, d,
                    cmd.hasHue ? cmd.hue : -1,
                    cmd.pattern, cmd.icon, cmd.emojiColor);
    } else {
      eye->setFace(p, a, d, cmd.hasHue ? cmd.hue : -1);
    }
    Serial.printf("MQTT: face -> P:%d A:%d D:%d\n", cmd.p, cmd.a, cmd.d);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Display + framebuffer
  canvas = new GFXcanvas16(240, 240);
  if (!canvas || !canvas->getBuffer()) {
    Serial.println("CANVAS ALLOC FAILED!");
    while (1) delay(1000);
  }
  framebuffer = canvas->getBuffer();
  memset(framebuffer, 0, 240 * 240 * sizeof(uint16_t));
  Serial.printf("Canvas OK. Free heap: %d\n", ESP.getFreeHeap());

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(0x0000);

  eye = new EyeRenderer(framebuffer, 240, 240);

  // Try to set timezone for time-of-day awareness
  configTzTime("America/Denver", "pool.ntp.org");

  eye->setMood(MOODS[0], 0);
  currentMoodName = MOODS[0].name;

  // Show connecting indicator
  eye->update();
  canvas->setTextSize(1);
  canvas->setTextColor(0x7BEF);
  canvas->setCursor(75, 220);
  canvas->print("connecting...");
  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);

  // Connect WiFi + MQTT
  mqtt.setFaceCallback(onFaceCommand);
  mqtt.setNowPlayingCallback([](const char *title, const char *artist, uint32_t durationMs) {
    startNowPlayingOverlay(title, artist, durationMs);
  });
  mqtt.setNeedsCallback([](const char *action) {
    if (strcmp(action, "feed") == 0) needs.feed();
    else if (strcmp(action, "pet") == 0) needs.pet();
    else if (strcmp(action, "play") == 0) needs.entertain();
    else if (strcasecmp(action, "off") == 0) crtOff();
    else if (strcasecmp(action, "on") == 0) crtOn();
    else return; // unknown action
    Serial.printf("Action: %s H:%d E:%d B:%d Joy:%d\n",
      action, (int)needs.getHunger(), (int)needs.getEnergy(),
      (int)needs.getBoredom(), (int)needs.getHappiness());
  });
  mqtt.begin();

  // OTA updates
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.onStart([]() {
    // Show update indicator on screen
    canvas->fillScreen(0x0000);
    canvas->setTextSize(2);
    canvas->setTextColor(0x07E0);
    canvas->setCursor(60, 110);
    canvas->print("Updating...");
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int pct = progress * 100 / total;
    canvas->fillRect(40, 140, 160, 10, 0x0000);
    canvas->fillRect(40, 140, pct * 160 / 100, 10, 0x07E0);
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.onEnd([]() {
    canvas->fillScreen(0x0000);
    canvas->setCursor(70, 110);
    canvas->setTextColor(0x07E0);
    canvas->print("Rebooting");
    tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);
  });
  ArduinoOTA.begin();

  Serial.printf("Ready. OTA hostname: %s. Free heap: %d\n", DEVICE_NAME, ESP.getFreeHeap());
}

void loop() {
  ArduinoOTA.handle();
  mqtt.loop();

  // When screen is off, only handle MQTT/OTA — don't render
  if (!screenOn) {
    delay(50); // save CPU
    return;
  }

  needs.update(millis());

  // When idle (no MQTT commands for 60s), let needs engine drive the face
  if (needs.shouldControl()) {
    int needsMoodIdx = needs.getCurrentMoodIndex();
    if (needsMoodIdx != lastNeedsMoodIdx) {
      lastNeedsMoodIdx = needsMoodIdx;
      currentMoodIndex = needsMoodIdx;
      currentMoodName = MOODS[needsMoodIdx].name;
      eye->setMood(MOODS[needsMoodIdx], needsMoodIdx);
      Serial.printf("Idle: %s (H:%d E:%d B:%d Joy:%d)\n",
        currentMoodName, (int)needs.getHunger(), (int)needs.getEnergy(),
        (int)needs.getBoredom(), (int)needs.getHappiness());
    }
  }

  // Render
  eye->update();

  // Mood name overlay — FreeSans 9pt, 50% opacity (mid-grey on dark bg)
  canvas->setFont(&FreeSans9pt7b);
  canvas->setTextSize(1);
  canvas->setTextWrap(false);
  // ~50% opacity white on black = grey
  uint16_t textCol = RGB565(120, 120, 120);
  int16_t x1, y1;
  uint16_t tw, th;
  canvas->getTextBounds(currentMoodName, 0, 0, &x1, &y1, &tw, &th);
  int16_t tx = (240 - tw) / 2 - x1;
  int16_t ty = 200;
  canvas->setCursor(tx, ty);
  canvas->setTextColor(textCol);
  canvas->print(currentMoodName);
  canvas->setFont(NULL); // reset to default for other text

  // Connection indicator (tiny dot top-right)
  uint16_t dotColor = mqtt.isConnected() ? 0x07E0 : (mqtt.isWifiConnected() ? 0xFFE0 : 0xF800);
  canvas->fillCircle(220, 20, 3, dotColor);

  // Now Playing overlay: dim the whole framebuffer (face + mood text + dot)
  // by the face alpha, then composite the track text on top.
  uint8_t faceAlpha = overlayFaceAlpha(millis());
  if (faceAlpha < 255) {
    dimFramebuffer(faceAlpha);
    renderNowPlayingText(255 - faceAlpha);
  }

  tft.drawRGBBitmap(0, 0, framebuffer, 240, 240);

  // Publish status periodically
  int8_t p, a, d;
  eye->getCurrentPAD(p, a, d);
  mqtt.publishStatus(p, a, d, currentMoodName, millis());
}
