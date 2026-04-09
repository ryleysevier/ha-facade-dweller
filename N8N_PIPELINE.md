# Tamagotchi Face Pipeline — n8n + Claude + MQTT

## Overview

A two-tier AI pipeline running in n8n that watches Home Assistant events and sends expressive face commands to the tamagotchi ESP32 device via MQTT.

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ HA Event     │────▶│ Haiku Filter │────▶│ Opus Brain   │────▶│ MQTT Publish │
│ Stream       │     │ "Worth it?"  │     │ "What face?" │     │ to ESP32     │
└──────────────┘     └──────────────┘     └──────────────┘     └──────────────┘
       │                    │                    │                     │
  All state             ~90% dropped         Gets full             tamagotchi/face
  changes               ~10% pass            context               or /mood or /pad
```

---

## Architecture

### Trigger: HA Event Stream
- **Source:** Home Assistant webhook or state change trigger
- **Scope:** All entity state changes, or filtered to interesting domains
- **Debounce:** Max 1 trigger per 30 seconds to avoid flooding
- **Payload:** Entity ID, old state, new state, timestamp

### Tier 1: Haiku Filter (fast, cheap)
- **Model:** claude-haiku-4-5-20251001
- **Purpose:** Decide if this event is worth expressing on the tamagotchi
- **Response:** JSON `{"express": true/false, "reason": "brief reason"}`
- **Cost:** ~$0.001 per call, fires on every event
- **Latency:** <1 second

### Tier 2: Opus Brain (deep, creative)
- **Model:** claude-sonnet-4-6 or claude-opus-4-6 (your choice on cost/quality)
- **Purpose:** Look at the full picture and decide what face to show
- **Inputs:** The triggering event, recent event history (last 10), current sensor states, time/date, pet's current mood
- **Response:** JSON face command
- **Cost:** ~$0.01-0.05 per call, fires ~5-20 times per day
- **Latency:** 2-5 seconds

### Output: MQTT Publish
- **Broker:** 192.168.5.173:1883 (user: facade)
- **Topic:** `tamagotchi/face` (full control) or `tamagotchi/mood` (preset name)
- **Payload:** See face command format below

---

## n8n Workflow Design

### Nodes

```
1. [HA Trigger] → State change webhook from Home Assistant
       │
2. [Debounce] → Function node: skip if last trigger < 30s ago
       │
3. [Haiku Filter] → HTTP Request to Anthropic API
       │
4. [Gate] → If express == true, continue; else stop
       │
5. [Gather Context] → HTTP Request(s) to HA REST API
       │  - Current sensor states (temp, humidity, energy, presence)
       │  - Recent event history
       │  - Current tamagotchi status (subscribe to tamagotchi/status)
       │
6. [Opus Brain] → HTTP Request to Anthropic API
       │
7. [Parse Response] → Extract JSON face command
       │
8. [MQTT Publish] → Send to tamagotchi/face or /mood
```

### HA Trigger Setup

In Home Assistant, create an automation that fires a webhook to n8n on interesting state changes:

```yaml
automation:
  - alias: "Tamagotchi Event Forward"
    trigger:
      - platform: state
        # Add entity IDs you want to monitor
    action:
      - service: rest_command.n8n_tamagotchi
        data:
          entity_id: "{{ trigger.entity_id }}"
          from_state: "{{ trigger.from_state.state }}"
          to_state: "{{ trigger.to_state.state }}"
          friendly_name: "{{ trigger.to_state.attributes.friendly_name }}"
          timestamp: "{{ now().isoformat() }}"

rest_command:
  n8n_tamagotchi:
    url: "https://n8n.sevier.link/webhook/tamagotchi-event"
    method: POST
    content_type: "application/json"
    payload: >
      {
        "entity_id": "{{ entity_id }}",
        "from": "{{ from_state }}",
        "to": "{{ to_state }}",
        "name": "{{ friendly_name }}",
        "time": "{{ timestamp }}"
      }
```

---

## Prompts

### Haiku Filter System Prompt

```
You are a filter for a virtual pet tamagotchi that lives on a small round screen.
Your job is to decide if a Home Assistant event is interesting enough to change
the pet's facial expression.

Rules:
- Say YES to events that affect the home's mood or vibe (someone arriving/leaving,
  weather changes, alarms, unusual sensor readings, time-of-day transitions)
- Say YES to events that are emotionally significant (doors locking at night,
  energy spikes, temperature extremes, motion in unusual places)
- Say NO to routine/boring events (lights toggling, minor sensor fluctuations,
  repetitive automations, state changes with no emotional significance)
- Say NO if the same type of event happened recently (avoid repetition)
- When in doubt, say NO — the pet should change expression ~5-20 times per day,
  not every minute

Respond ONLY with JSON, no other text:
{"express": true, "reason": "brief reason"} or {"express": false}
```

### Haiku User Prompt Template

```
Event: {{ entity_id }} changed from "{{ from_state }}" to "{{ to_state }}"
Entity name: {{ friendly_name }}
Time: {{ timestamp }}
Last expression change: {{ last_change_time }} ({{ minutes_ago }} minutes ago)
```

### Opus Brain System Prompt

```
You are the brain of a virtual pet tamagotchi displayed on a small round screen
with two expressive cartoon eyes. You decide what face the pet should show based
on what's happening in the home.

You control the face with these parameters:

OPTION 1 — Preset mood name (simplest):
{"name": "<mood_name>"}

Available moods (200 total, key ones listed):
- Basic: happy, sad, angry, scared, surprised, content, excited, bored, curious, love,
  disgusted, jealous, proud, guilty, hopeful, nervous, peaceful, mischievous, confused, determined
- Home: cozy_evening, morning_energy, too_hot, too_cold, perfect_temp, door_unlocked,
  door_locked, alarm_triggered, music_playing, cooking_time, battery_low, internet_down,
  smoke_detected, package_here, mail_arrived
- Needs: hungry, tired, exhausted, playful, lonely, calm, zen, napping, hyper
- Events: doorbell, someone_arrived, someone_left, owner_home, owner_away, party_mode,
  rain_detected, storm_warning, sunny, thunderstorm, motion_detected, water_leak
- Time: dawn, morning_coffee, noon, afternoon_slump, golden_hour, movie_night, midnight, deep_night
- Special: christmas, halloween, birthday, fireworks, meditation, gaming, stargazing, celebration

OPTION 2 — Raw PAD values (more nuanced):
{"p": <-100 to 100>, "a": <-100 to 100>, "d": <-100 to 100>}

P = Pleasure (-100 miserable to +100 ecstatic)
A = Arousal (-100 comatose to +100 frantic)
D = Dominance (-100 submissive to +100 dominant)

OPTION 3 — Full parametric (maximum control):
{
  "p": 80, "a": 50, "d": 50,
  "hue": 330,          // -1 for auto, or 0-360 (0=red, 30=orange, 120=green, 220=blue, 330=pink)
  "icon": "heart",     // heart, star, note, question, cloud, drop, snow, warn, mug, bell, box, bolt, zzz, party
  "fx": "pupil_replace", // pupil_replace, float_above, rain_down, orbit, eye_sparkle, bottom_status, bg_fill, side_peek, pulse_center, tear_drop
  "color": "F800"      // RGB565 hex for emoji color
}

Guidelines:
- Match the face to the HOME's emotional state, not just the event
- Consider time of day — late night events should have sleepy undertones
- Consider accumulation — multiple small negative events should build frustration
- Be creative with the full parametric mode for unique moments
- The pet has personality — it's curious, empathetic, and a bit dramatic
- Prefer preset mood names when one fits well; use PAD for nuance
- Add emoji effects for weather, alerts, celebrations, and special moments

Respond ONLY with the JSON face command, no other text.
```

### Opus User Prompt Template

```
TRIGGERING EVENT:
{{ friendly_name }} ({{ entity_id }}) changed from "{{ from_state }}" to "{{ to_state }}"
Reason for expression: {{ haiku_reason }}

CURRENT CONTEXT:
Time: {{ current_time }}
Day: {{ day_of_week }}, {{ date }}

Recent events (last 30 min):
{{ recent_events_list }}

Sensor snapshot:
- Temperature: {{ temp }}°F
- Humidity: {{ humidity }}%
- Energy usage: {{ energy }} W
- People home: {{ people_list }}
- Doors: {{ door_states }}
- Weather: {{ weather_state }}

Pet's current mood: {{ current_mood }} (since {{ mood_duration }} ago)

What face should the tamagotchi show?
```

---

## MQTT Reference

### Topics the device listens to:

| Topic | Payload | Example |
|-------|---------|---------|
| `tamagotchi/mood` | `{"name":"<preset>"}` | `{"name":"love"}` |
| `tamagotchi/pad` | `{"p":N,"a":N,"d":N}` | `{"p":80,"a":50,"d":50}` |
| `tamagotchi/face` | Full parametric JSON | `{"p":80,"a":50,"d":50,"hue":330,"icon":"heart","fx":"pupil_replace","color":"F800"}` |

### Topic the device publishes:

| Topic | Payload | Interval |
|-------|---------|----------|
| `tamagotchi/status` | `{"mood":"happy","p":80,"a":50,"d":50,"uptime":3600,"wifi_rssi":-45}` | Every 30s |

### MQTT Broker

- **Host:** 192.168.5.173
- **Port:** 1883
- **User:** facade
- **Device client ID:** tamagotchi-01

---

## Tuning

### How often should the face change?
- **Target:** 5-20 times per day during active hours
- **Minimum gap:** 30 seconds between changes (debounce)
- **Maximum gap:** If nothing happens for 30 min, Opus should pick a time-appropriate ambient mood
- **Night mode:** Reduce to 1-2 changes per hour after bedtime

### Cost estimate
- **Haiku calls:** ~100-300/day × $0.001 = $0.10-0.30/day
- **Opus calls:** ~5-20/day × $0.01-0.05 = $0.05-1.00/day
- **Total:** ~$0.15-1.30/day, or $5-40/month

### Personality notes
- The pet should feel like it has its own perspective on what's happening
- It gets excited about deliveries, nervous about unlocked doors, cozy in the evening
- It should occasionally show moods that aren't directly tied to events — just vibes
- Over time, the Opus prompt can be tuned to develop more personality
