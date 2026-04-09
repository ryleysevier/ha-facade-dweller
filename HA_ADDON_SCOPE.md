# Home Assistant Add-on Scope: Tamagotchi Pet Manager

## Vision

Move the pet's brain off the ESP32 and into Home Assistant as a dedicated add-on. The ESP32 becomes a pure display peripheral — a "face terminal" that receives draw commands via MQTT. All intelligence, personality, needs tracking, and AI integration lives in HA where it has full access to sensors, history, calendar, and Claude.

## Why an Add-on (vs current split architecture)

| Aspect | Current (ESP32 brain) | Add-on (HA brain) |
|--------|----------------------|-------------------|
| Needs engine | Runs on ESP32, limited context | Runs in HA, sees all sensors |
| Time awareness | NTP only | HA calendar, sunrise/sunset, schedules |
| AI pipeline | Separate n8n workflow | Built into add-on, direct HA API access |
| Pet state persistence | Lost on reboot | Stored in HA, survives reboots |
| Multiple displays | Each runs its own pet | One pet, multiple display endpoints |
| User interaction | MQTT only | HA dashboard + MQTT + voice |
| Configuration | Edit C++ and reflash | HA UI config panel |

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    HOME ASSISTANT                        │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │          TAMAGOTCHI ADD-ON (Python)                 │  │
│  │                                                     │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────┐ │  │
│  │  │ Pet State    │  │ AI Brain     │  │ Event    │ │  │
│  │  │ Manager      │  │ (Claude API) │  │ Watcher  │ │  │
│  │  │              │  │              │  │          │ │  │
│  │  │ hunger       │  │ Haiku filter │  │ State    │ │  │
│  │  │ energy       │  │ Opus decide  │  │ changes  │ │  │
│  │  │ boredom      │  │ Personality  │  │ Calendar │ │  │
│  │  │ happiness    │  │ Memory       │  │ Weather  │ │  │
│  │  │ personality  │  │              │  │ Presence │ │  │
│  │  └──────┬───────┘  └──────┬───────┘  └────┬─────┘ │  │
│  │         │                 │                │       │  │
│  │         └────────┬────────┴────────────────┘       │  │
│  │                  │                                  │  │
│  │         ┌────────▼────────┐                        │  │
│  │         │ MQTT Publisher  │                        │  │
│  │         │ tamagotchi/*    │                        │  │
│  │         └────────┬────────┘                        │  │
│  └──────────────────┼─────────────────────────────────┘  │
│                     │                                     │
│  ┌──────────────────▼─────────────────────────────────┐  │
│  │          HA INTEGRATION                             │  │
│  │  - sensor.tamagotchi_hunger (0-100)                 │  │
│  │  - sensor.tamagotchi_happiness (0-100)              │  │
│  │  - sensor.tamagotchi_mood (text)                    │  │
│  │  - button.tamagotchi_feed                           │  │
│  │  - button.tamagotchi_pet                            │  │
│  │  - button.tamagotchi_play                           │  │
│  │  - select.tamagotchi_mood_override (dropdown)       │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                          │
│  ┌─────────────────────────────────────────────────────┐  │
│  │          LOVELACE DASHBOARD CARD                    │  │
│  │  - Live eye preview (canvas, same as web preview)   │  │
│  │  - Need bars (hunger, energy, boredom, happiness)   │  │
│  │  - Feed/Pet/Play buttons                            │  │
│  │  - Mood history graph                               │  │
│  │  - AI reasoning log ("changed to cozy because...")  │  │
│  └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │ MQTT
                          ▼
                 ┌──────────────────┐
                 │  ESP32 + GC9A01  │
                 │  (dumb display)  │
                 │  Just renders    │
                 │  face commands   │
                 └──────────────────┘
```

## Add-on Components

### 1. Pet State Manager
- Persistent state stored in `/data/pet_state.json`
- Needs decay rates configurable via HA UI
- Tracks: hunger, energy, boredom, happiness, loneliness
- Tracks: personality traits (develops over time based on interactions)
- Tracks: mood history (last 100 mood changes with timestamps + reasons)
- Publishes HA sensors for dashboard visibility

### 2. AI Brain
- **Haiku filter**: Evaluates HA event stream, decides what's worth expressing
- **Opus/Sonnet brain**: Crafts face commands with full context
- **Personality layer**: Consistent character that develops over time
- **Memory**: Remembers recent events, learns preferences (e.g., "owner likes when I react to packages")
- **Prompt templates**: Configurable via HA UI
- Anthropic API key configured in add-on settings

### 3. Event Watcher
- Subscribes to HA event bus (state_changed events)
- Filters to configured entity domains/IDs
- Debounces (configurable, default 30s)
- Feeds events to Haiku filter
- Also watches: time patterns, calendar events, weather changes, person tracking

### 4. MQTT Publisher
- Sends face commands to ESP32 via existing MQTT topics
- Publishes pet status for dashboard
- Listens for feed/pet/play commands from dashboard buttons

### 5. HA Integration (custom component)
- Exposes pet state as HA entities (sensors, buttons, selects)
- Enables automations ("when tamagotchi is hungry, send notification")
- Enables voice control ("Hey Google, feed the tamagotchi")

### 6. Lovelace Card (optional, future)
- Custom card showing the animated eyes in the browser
- Uses same canvas rendering as tools/preview.html
- Shows need bars, buttons, mood history
- Could be the "second screen" — phone shows pet while desk display shows it too

## Configuration (HA UI)

```yaml
# Add-on config
anthropic_api_key: "sk-ant-..."
mqtt_topic_prefix: "tamagotchi"
display_devices:
  - name: "Desk Pet"
    client_id: "tamagotchi-01"
pet_name: "Buddy"
personality: "curious, empathetic, slightly dramatic"
watched_domains:
  - person
  - lock
  - climate
  - weather
  - binary_sensor
  - sensor.energy
ai_model_filter: "claude-haiku-4-5-20251001"
ai_model_brain: "claude-sonnet-4-6"
max_face_changes_per_hour: 12
quiet_hours:
  start: "23:00"
  end: "06:00"
need_decay_rates:
  hunger: 0.014    # per second
  boredom: 0.055
  loneliness: 0.028
```

## ESP32 Firmware Changes (if going this route)

The ESP32 firmware would be SIMPLIFIED:
- Remove `needs.h/cpp` (needs engine moves to add-on)
- Remove demo mode cycling
- Keep: eye renderer, emoji fx, MQTT handler
- Add: boot animation, "waiting for connection" face
- Add: OTA update support
- Essentially becomes a ~300 line "face terminal"

## Existing Art / Inspiration

- **MACS**: Animated companion for HA (voice assistant face, no persistent state)
  - https://community.home-assistant.io/t/meet-macs-a-playful-expressive-animated-companion-for-home-assistant/975092
- **HA Digital Pet Dashboard**: Community concept thread (no implementation)
  - https://community.home-assistant.io/t/home-assistant-digital-pet-dashboard-tamagotchi/784294
- **Habitica Integration**: RPG gamification (human tasks, not pet)
- **FloraCare**: Plant tamagotchi with sensor feedback (ESP32-S3, closest hardware concept)

## Implementation Phases

### Phase 1: Basic Add-on
- Python add-on with needs engine (port from ESP32 C++)
- MQTT publisher (same topics)
- Config via add-on options
- ESP32 firmware simplified to face terminal

### Phase 2: HA Integration
- Custom component exposing sensors/buttons
- Dashboard card with need bars + controls
- Basic automations (notify when hungry, etc.)

### Phase 3: AI Brain
- Haiku filter on event stream
- Opus brain for face decisions
- Personality system + memory
- Configurable prompts

### Phase 4: Lovelace Card
- Animated eye preview in browser
- Full pet management UI
- Mood history timeline
- AI reasoning log

### Phase 5: Community Release
- HACS compatible
- Documentation
- Multiple display support
- Custom personality templates
