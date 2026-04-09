# Face Creation Guide

## Animation principles (what makes eyes look alive)

### Shape language
- **Round eyes** = innocent, friendly, surprised
- **Narrow/angular** = menacing, suspicious, angry
- **Droopy** = tired, sad, bored
- **Sharp corners** = aggressive; **soft corners** = gentle

### Sclera (white space around pupil)
The single biggest emotional signal after shape:
- **Tiny pupil in big eye** (max sclera) = fear, surprise, stress
- **Big pupil filling eye** (min sclera) = love, interest, calm
- **Contracted pupil** = anger, focus, disgust

### Squash & stretch
- **Angry** = squash (wide + flat)
- **Surprised/scared** = stretch (tall + narrow)
- **Happy** = round
- **Bored** = tiny flat slits

### Timing is everything
- **Fast blinks** = nervous. **Slow blinks** = confident/sleepy.
- **Blink asymmetry** = close fast, open slow (follow-through)
- **Anticipation** = slight squint before blink, pause before gaze shift
- **Saccade overshoot** = eyes dart past target, settle back (3-phase: position → overshoot 15% → settle)

### Specular highlights (catchlights)
- Essential for life — without them eyes look dead
- Primary highlight: large, upper-left area of pupil
- Secondary highlight: small, offset — adds depth
- Highlights shift opposite to gaze direction (light source is fixed)

### Micro-movements
- Random saccades every 80-300ms (tiny eye jitters)
- Idle gaze drift every 1-3 seconds
- Subtle breathing oscillation on eye size
- Slight asymmetry between left/right eye (~3% size difference)

### Sources
- Disney's 12 Principles of Animation (squash/stretch, anticipation, follow-through)
- Cozmo/Vector robot emotion engine (GDC talk by Anki)
- Live2D VTuber parameter standard (6-8 core eye params)
- FACS: AU5 (lid raise), AU6 (smile squeeze), AU7 (anger tighten)
- PAD Model: Mehrabian & Russell 1974

---

## How the system works

Every face is defined by **3 numbers** (PAD emotional model):

- **Pleasure** (-100 to 100): How good/bad. Positive = happy, warm. Negative = sad, angry.
- **Arousal** (-100 to 100): How energized. Positive = alert, jittery. Negative = calm, sleepy.
- **Dominance** (-100 to 100): How in-control. Positive = confident, direct gaze. Negative = submissive, averted.

The renderer procedurally maps these 3 values to all visual parameters:

| PAD Value | Eye Effect |
|-----------|-----------|
| High Pleasure | Round shape, warm gold hue, big pupils, happy squint |
| Low Pleasure | Angular shape, cool/red hue, small pupils, V-lids |
| High Arousal | Big eyes, dilated pupils, fast blinks, jittery saccades |
| Low Arousal | Small/squinted eyes, droopy lids, slow blinks, still |
| High Dominance | Wide eyes, direct gaze, sharp corners |
| Low Dominance | Averted gaze, rounded, looking down |

### Key emotion recipes

| Emotion | P | A | D | Why |
|---------|---|---|---|-----|
| Happy | +80 | +50 | +50 | Warm, energized, confident |
| Sad | -70 | -40 | -50 | Cold, low energy, submissive |
| Angry | -60 | +80 | +70 | Cold, very energized, dominant → red, squashed, V-lids |
| Scared | -70 | +90 | -80 | Cold, very energized, very submissive → purple, stretched tall, tiny pupils |
| Bored | -30 | -60 | 0 | Slightly negative, very low energy → teal slits |
| Love | +95 | +10 | -10 | Very warm, calm, soft → big pupils, round, amber |

## Adding a new face

### 1. Edit `src/moods_data.h`

Add a new entry to the `MOODS` array:

```c
/* 200 */ {"my_new_mood",  P, A, D},
```

Update `NUM_MOODS` in `src/moods.h` if adding beyond 200.

### 2. Preview it

**Web preview (fastest):**
```
open tools/preview.html
```
- Use the PAD sliders to dial in the look
- Note the P/A/D values when it looks right
- Or click existing presets and tweak from there

**Export grid images:**
```
python3 tools/export_emotions.py
```
Generates `emotion_previews/batch_XX_*.png` — 5x4 grids of 20 moods each.

### 3. Add a hue override (optional)

If the PAD-derived color doesn't fit (e.g., a weather mood that should be blue but PAD makes it amber), add an override in `src/eye_renderer.cpp` in the `applyHueOverride()` function:

```cpp
case 200: params.hue = 210; break; // my_new_mood: steel blue
```

Category overrides are grouped: time-of-day (130-159), weather, holidays, home utility, activities.

### 4. Add an emoji effect (optional)

If the mood benefits from an icon overlay, add it in `src/emoji_fx.cpp` in `getMoodEmoji()`:

```cpp
case 200: // my_new_mood
  effect = {EMOJI_PATTERN, ICON_ID, ICON_ID_COUNT, 0xFFFF};
  return true;
```

Also add it to the `EMOJI_MAP` in `tools/preview.html` for web preview.

**Available patterns:**

| Pattern | Effect | Good for |
|---------|--------|----------|
| `EMOJI_PUPIL_REPLACE` | Icon replaces pupils | love (hearts), stargazing (stars) |
| `EMOJI_FLOAT_ABOVE` | Icon bobs above eyes | sleep (Zzz), weather (sun/cloud) |
| `EMOJI_RAIN_DOWN` | Icons fall from top | rain, snow, water leak |
| `EMOJI_ORBIT` | Icons circle the face | confused (?), windy (clouds) |
| `EMOJI_EYE_SPARKLE` | Icons flash at highlights | excited, fireworks |
| `EMOJI_BOTTOM_STATUS` | Icon below eyes | coffee, reading, campfire |
| `EMOJI_BG_FILL` | Scattered in background | party, music, holidays |
| `EMOJI_SIDE_PEEK` | Icon peeks from edge | doorbell, package, mail |
| `EMOJI_PULSE_CENTER` | Icon throbs between eyes | alarm, warning, battery |
| `EMOJI_TEAR_DROP` | Icon slides from eye corner | sad, nervous, hot/sweat |

**Available icons:**

`ICON_ID_HEART`, `ICON_ID_STAR`, `ICON_ID_NOTE`, `ICON_ID_QUESTION`, `ICON_ID_CLOUD`, `ICON_ID_DROP`, `ICON_ID_SNOW`, `ICON_ID_WARN`, `ICON_ID_MUG`, `ICON_ID_BELL`, `ICON_ID_BOX`, `ICON_ID_BOLT`, `ICON_ID_ZZZ`, `ICON_ID_PARTY`

To add a new icon: edit `src/icons.h`, add a 12x12 bitmap array and register it in `ICON_DATA[]` and `IconId` enum.

### 5. Build and flash

```bash
pio run -t upload
```

## Tools

| Tool | Command | Purpose |
|------|---------|---------|
| Web preview | `open tools/preview.html` | Live interactive preview with PAD sliders, all 200 presets, arrow key navigation, space=autoplay, E=jump to emoji moods |
| Export grids | `python3 tools/export_emotions.py` | Generates 10 batch PNG grids (20 moods each) for visual review |
| Build + flash | `pio run -t upload` | Compile and upload to ESP32-C6 |
| Serial monitor | `pio device monitor` | See mood names + PAD values as they cycle |

## File map

| File | Purpose |
|------|---------|
| `src/moods.h` | `Mood` struct (name + PAD), `NUM_MOODS` |
| `src/moods_data.h` | All 200 mood PAD values |
| `src/eye_renderer.h/cpp` | PAD→eye parameter mapper + renderer + hue overrides |
| `src/emoji_fx.h/cpp` | Emoji animation patterns + mood→emoji mapping |
| `src/icons.h` | 12x12 pixel art icon bitmaps |
| `src/main.cpp` | Setup + mood cycling loop |
| `tools/preview.html` | Web preview tool |
| `tools/export_emotions.py` | Batch grid image exporter |

## Tips

- **Similar moods look the same?** Spread their PAD values further apart, add a hue override, or add an emoji effect.
- **Mood color wrong?** Add a `case` in `applyHueOverride()`. Hue is 0-360 (0=red, 30=orange, 60=yellow, 120=green, 180=teal, 220=blue, 280=purple, 330=pink).
- **Need a new icon?** Draw it at 12x12 in any pixel editor, convert to a 2-byte-per-row C array, add to `icons.h`.
- **Testing one specific mood?** In the web preview, click it or use arrow keys to find it. Moods with emoji effects have a red border in the sidebar.
- **The PAD model source:** Mehrabian & Russell (1974). All emotions map to these 3 axes. The third axis (Dominance) is what distinguishes anger from fear — both are negative pleasure + high arousal, but anger is high dominance and fear is low.
