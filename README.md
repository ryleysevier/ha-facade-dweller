# Facade — A Home Assistant Tamagotchi

An expressive virtual pet that lives on a 1.28" round display, reacts to your smart home, and has its own emotional life.

![Facade Eyes](https://img.shields.io/badge/moods-200-blue) ![ESP32](https://img.shields.io/badge/ESP32--C6-PlatformIO-orange) ![MQTT](https://img.shields.io/badge/MQTT-connected-green)

## What is this?

A round LCD screen with two cartoon eyes that express emotion using the **PAD emotional model** (Pleasure-Arousal-Dominance). The eyes change shape, color, size, and animation based on 3 numbers — producing 200+ distinct expressions procedurally.

**Features:**
- 200 mood presets covering emotions, home states, weather, time-of-day, holidays, and pet needs
- PAD-driven procedural eye rendering (squash/stretch, sclera visibility, shape language)
- 14 pixel art emoji icons with 10 animation patterns (rain, orbiting, pupil replace, etc.)
- Autonomous needs engine (hunger, boredom, loneliness, energy cycles)
- MQTT control — set faces from Home Assistant, n8n, or any MQTT client
- Web preview tool with live animation and all 200 presets
- MQTT control panel for interactive testing

## Hardware

- **MCU:** [Seeed Studio XIAO ESP32-C6](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
- **Display:** [Elecrow 1.28" Round LCD (GC9A01, 240x240)](https://www.elecrow.com/1-28-inch-round-lcd-module-gc9a01-240x240-lcd-display.html)

### Wiring

| LCD Pin | XIAO Pin | GPIO |
|---------|----------|------|
| VCC | 3V3 | — |
| GND | GND | — |
| SDA | D10 | 18 (MOSI) |
| SCL | D8 | 19 (SCK) |
| CS | D1 | 1 |
| DC | D0 | 0 |
| RST | D2 | 2 |

## Setup

### 1. Clone and configure

```bash
git clone https://github.com/ryleysevier/facade-display.git
cd facade-display
cp .env.example .env
# Edit .env with your WiFi and MQTT credentials
```

### 2. Build and flash

```bash
# Load env vars and build
export $(cat .env | xargs) && pio run -t upload
```

### 3. Test

```bash
# Set a mood
mosquitto_pub -h YOUR_MQTT_HOST -u YOUR_USER -P 'YOUR_PASS' -t tamagotchi/mood -m '{"name":"love"}'

# Send raw PAD values
mosquitto_pub -h YOUR_MQTT_HOST -u YOUR_USER -P 'YOUR_PASS' -t tamagotchi/pad -m '{"p":80,"a":50,"d":50}'

# Full parametric control
mosquitto_pub -h YOUR_MQTT_HOST -u YOUR_USER -P 'YOUR_PASS' -t tamagotchi/face -m '{"p":80,"a":50,"d":50,"hue":330,"icon":"heart","fx":"pupil_replace","color":"F800"}'

# Feed the pet
mosquitto_pub -h YOUR_MQTT_HOST -u YOUR_USER -P 'YOUR_PASS' -t tamagotchi/feed -m ''
```

## MQTT Topics

### Control (subscribe)

| Topic | Payload | Description |
|-------|---------|-------------|
| `tamagotchi/mood` | `{"name":"happy"}` | Set from 200 presets |
| `tamagotchi/pad` | `{"p":80,"a":50,"d":50}` | Raw PAD values |
| `tamagotchi/face` | `{"p":80,"a":50,"d":50,"hue":330,"icon":"heart","fx":"pupil_replace"}` | Full parametric |
| `tamagotchi/feed` | any | Feed the pet |
| `tamagotchi/pet` | any | Pet it |
| `tamagotchi/play` | any | Entertain it |

### Status (publish)

| Topic | Payload | Interval |
|-------|---------|----------|
| `tamagotchi/status` | `{"mood":"happy","p":80,"a":50,"d":50,"uptime":3600}` | 30s |

## Tools

| Tool | Usage |
|------|-------|
| `tools/preview.html` | Live eye preview with PAD sliders and all 200 presets. Arrow keys to step, Space to autoplay, E to jump to emoji moods. |
| `tools/control.html` | MQTT control panel with care buttons, mood presets, PAD sliders, and raw MQTT sender. |
| `tools/export_emotions.py` | Generate grid preview images of all 200 moods. |

## How It Works

See [FACES_GUIDE.md](FACES_GUIDE.md) for the full face creation guide, including PAD model explanation, animation principles, and how to add new moods/emojis.

## Architecture

The companion Home Assistant add-on lives at [ryleysevier/ha-facade](https://github.com/ryleysevier/ha-facade). See [HA_ADDON_SCOPE.md](HA_ADDON_SCOPE.md) for the full add-on architecture.

See [N8N_PIPELINE.md](N8N_PIPELINE.md) for the two-tier AI pipeline (Haiku filter + Opus brain) that connects Home Assistant events to face expressions.

## License

MIT
