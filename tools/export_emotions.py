#!/usr/bin/env python3
"""Export the 10 basic emotions as PNG images for review."""

import math
import os
from PIL import Image, ImageDraw

SIZE = 240
OUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'emotion_previews')

EMOTIONS = [
    ("happy",      0.80,  0.50,  0.50),
    ("sad",       -0.70, -0.40, -0.50),
    ("angry",     -0.60,  0.80,  0.70),
    ("scared",    -0.70,  0.90, -0.80),
    ("surprised",  0.10,  0.95, -0.40),
    ("content",    0.70, -0.30,  0.40),
    ("excited",    0.90,  0.90,  0.60),
    ("bored",     -0.30, -0.60,  0.00),
    ("curious",    0.40,  0.50, -0.10),
    ("love",       0.95,  0.40,  0.20),
]

def clamp(v, lo, hi):
    return max(lo, min(hi, v))

def hsv_to_rgb(h, s, v):
    c = v * s
    x = c * (1 - abs((h / 60) % 2 - 1))
    m = v - c
    if h < 60:    r,g,b = c,x,0
    elif h < 120: r,g,b = x,c,0
    elif h < 180: r,g,b = 0,c,x
    elif h < 240: r,g,b = 0,x,c
    elif h < 300: r,g,b = x,0,c
    else:         r,g,b = c,0,x
    return (int((r+m)*255), int((g+m)*255), int((b+m)*255))

def pad_to_eye(p, a, d):
    p = clamp(p, -1, 1)
    a = clamp(a, -1, 1)
    d = clamp(d, -1, 1)

    base = 130
    height = base + a*35 + p*10
    width = base + a*15 + p*10 + d*10
    if p < -0.3 and a > 0.3 and d > 0:
        width += 20; height -= 30
    if p < -0.3 and a > 0.3 and d < -0.3:
        height += 20; width -= 10

    roundness = 0.65 + p*0.25
    if p < -0.3 and a > 0.3: roundness = 0.2
    if a > 0.7 and p >= 0: roundness = 0.95
    if p > 0.5: roundness = 0.85
    roundness = clamp(roundness, 0.15, 1.0)

    openness = 0.7 + a*0.25
    if a < -0.5: openness -= 0.2
    if p < -0.3 and a > 0.3 and d > 0: openness = 0.5
    if a > 0.8: openness = 1.0
    openness = clamp(openness, 0.12, 1.0)

    pupil_size = 0.35 + p*0.08
    if p > 0.5: pupil_size = 0.5
    if p < -0.3 and a > 0.5 and d < 0: pupil_size = 0.18
    if p < -0.3 and a > 0.3 and d > 0: pupil_size = 0.22
    if a > 0.7 and p >= 0: pupil_size = 0.25
    pupil_size = clamp(pupil_size, 0.12, 0.6)

    gaze_x = -0.2 if d < -0.3 else 0
    gaze_y = d * -0.15

    lid_top = 0
    if a < 0: lid_top = abs(a) * 0.5
    if p < -0.3 and a > 0.3: lid_top = 0.45

    lid_bottom = 0
    if p > 0.3: lid_bottom = p * 0.4
    if a > 0.7 and p >= 0: lid_bottom = 0

    lid_angle = 0
    if p < -0.3 and a > 0.3: lid_angle = -0.6 - p * 0.3
    if p < 0 and a < -0.3: lid_angle = 0.4 + abs(a) * 0.2
    if d > 0.5 and p < 0: lid_angle -= 0.2
    lid_angle = clamp(lid_angle, -1, 1)

    # Brows
    brow_height = clamp(0.3 + a*0.3, -0.5, 1.0)
    if p < -0.3 and a > 0.3 and d > 0: brow_height = -0.2
    if p < -0.5 and d < -0.3: brow_height = 0.7
    brow_height = clamp(brow_height, -0.5, 1.0)

    brow_angle = 0
    if p < -0.3 and a > 0.3: brow_angle = -0.6 - d * 0.3
    if p < -0.3 and d < -0.3: brow_angle = 0.5 + abs(d) * 0.3
    if p < -0.3 and a < -0.3: brow_angle = 0.3
    if p > 0.5: brow_angle = 0.1
    brow_angle = clamp(brow_angle, -1, 1)

    brow_thickness = clamp(0.5 + d*0.2, 0.3, 1.0)
    if p < -0.3 and a > 0.5: brow_thickness = 0.8

    brow_curve = clamp(a*0.6 + p*0.2, -1, 1)
    if a > 0.7: brow_curve = 0.8
    if a < -0.5: brow_curve = -0.4

    # Hue
    if p > 0.3 and a > 0.3: hue = 40
    elif p > 0.3: hue = 30
    elif p < -0.3 and a > 0.3 and d > 0: hue = 5
    elif p < -0.3 and a > 0.3: hue = 280
    elif p < -0.3 and a <= 0: hue = 220
    elif a < -0.3: hue = 180
    else: hue = 35

    sat = clamp(0.6 + abs(a)*0.3 - (0.2 if (p<-0.5 and d<-0.3) else 0), 0.3, 1.0)
    bri = clamp(0.7 + p*0.2, 0.4, 1.0)
    glow = clamp(abs(a)*0.5 + (p*0.3 if p>0 else 0), 0.0, 0.8)

    return dict(width=width, height=height, roundness=roundness, openness=openness,
                pupil_size=pupil_size, gaze_x=gaze_x, gaze_y=gaze_y,
                lid_top=lid_top, lid_bottom=lid_bottom, lid_angle=lid_angle,
                brow_height=brow_height, brow_angle=brow_angle,
                brow_thickness=brow_thickness, brow_curve=brow_curve,
                hue=hue, sat=sat, bri=bri, glow=glow)


def render_eye(name, p, a, d):
    e = pad_to_eye(p, a, d)
    img = Image.new('RGB', (SIZE, SIZE), (0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx, cy = SIZE // 2, SIZE // 2
    gap = 12
    eye_w = e['width'] * 0.42
    eye_h = e['height'] * 0.55 * e['openness']
    eye_h = max(3, eye_h)
    rr = e['roundness'] * min(eye_w, eye_h) * 0.5

    lcx = cx - gap/2 - eye_w/2
    rcx = cx + gap/2 + eye_w/2

    eye_col = hsv_to_rgb(e['hue'], e['sat'], e['bri'])
    bg_col = hsv_to_rgb(e['hue'], e['sat']*0.15, 0.05)
    glow_col = hsv_to_rgb(e['hue'], e['sat']*0.6, e['bri']*0.3*e['glow'])
    brow_col = hsv_to_rgb(e['hue'], e['sat']*0.4, e['bri']*0.35)

    # Background
    draw.rectangle([0, 0, SIZE, SIZE], fill=bg_col)

    # Circular mask
    mask = Image.new('L', (SIZE, SIZE), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.ellipse([2, 2, SIZE-3, SIZE-3], fill=255)

    # Glow
    if e['glow'] > 0.05:
        gr = int(e['glow'] * 50 + eye_w * 0.5)
        for r in range(gr, 0, -3):
            t = r / gr
            gc = tuple(int(bg_col[i] + (glow_col[i] - bg_col[i]) * t * t) for i in range(3))
            draw.ellipse([cx-r, cy-r, cx+r, cy+r], fill=gc)

    for side in range(2):
        ecx = lcx if side == 0 else rcx
        mirror = 1 if side == 0 else -1

        ex = ecx - eye_w/2
        ey = cy - eye_h/2

        # Eye body
        draw.rounded_rectangle([ex, ey, ex+eye_w, ey+eye_h], radius=rr, fill=eye_col)

        # Pupil
        if eye_h > 8:
            pup_r = e['pupil_size'] * min(eye_w, eye_h) * 0.45
            pup_r = max(4, pup_r)
            mgx = eye_w * 0.5 - pup_r - 3
            mgy = eye_h * 0.5 - pup_r - 3
            px = ecx + e['gaze_x'] * mgx
            py = cy + e['gaze_y'] * mgy

            draw.ellipse([px-pup_r, py-pup_r, px+pup_r, py+pup_r], fill=bg_col)
            core_r = max(2, pup_r * 0.4)
            draw.ellipse([px-core_r, py-core_r, px+core_r, py+core_r], fill=(0,0,0))
            hl_r = max(1, pup_r * 0.22)
            hlx = px - pup_r * 0.25
            hly = py - pup_r * 0.25
            draw.ellipse([hlx-hl_r, hly-hl_r, hlx+hl_r, hly+hl_r], fill=(255,255,255))

        # Top lid: angry (lid_angle negative) = inner corners come DOWN
        if e['lid_top'] > 0.01 or abs(e['lid_angle']) > 0.01:
            droop_px = e['lid_top'] * eye_h * 0.6
            raw_lid_angle_px = e['lid_angle'] * eye_w * 0.3
            inner_x = (ex + eye_w + 6) if side == 0 else (ex - 6)
            outer_x = (ex - 6) if side == 0 else (ex + eye_w + 6)
            inner_droop = max(0, droop_px + (-raw_lid_angle_px))
            outer_droop = max(0, droop_px + raw_lid_angle_px)
            pts = [
                (inner_x, ey - 6),
                (outer_x, ey - 6),
                (outer_x, ey + outer_droop),
                (inner_x, ey + inner_droop),
            ]
            draw.polygon(pts, fill=bg_col)

        # Bottom lid
        if e['lid_bottom'] > 0.01:
            raise_px = e['lid_bottom'] * eye_h * 0.5
            eye_bot = ey + eye_h
            pts = [
                (ex - 6, eye_bot + 6),
                (ex + eye_w + 6, eye_bot + 6),
                (ecx, eye_bot - raise_px),
            ]
            draw.polygon(pts, fill=bg_col)

        # Eyebrow (disabled for now)

    # Apply circular mask
    bg = Image.new('RGB', (SIZE, SIZE), (0, 0, 0))
    img = Image.composite(img, bg, mask)

    return img


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    # Import all 200 moods from moods_data.h PAD values
    ALL_MOODS = [
        ("happy",80,50,50),("sad",-70,-40,-50),("angry",-60,80,70),("scared",-70,90,-80),
        ("surprised",10,95,-40),("content",70,-30,40),("excited",90,90,60),("bored",-30,-60,0),
        ("curious",40,50,-10),("love",95,40,20),("disgusted",-80,40,50),("jealous",-50,50,30),
        ("proud",70,30,80),("guilty",-60,-20,-60),("hopeful",60,30,-10),("nervous",-30,70,-50),
        ("peaceful",60,-70,20),("mischievous",30,60,50),("confused",-20,40,-40),("determined",40,50,80),
        ("cozy_evening",75,-40,30),("morning_energy",60,60,40),("too_hot",-40,50,-10),("too_cold",-40,30,-30),
        ("perfect_temp",70,-20,30),("door_unlocked",-20,60,-30),("door_locked",50,-30,50),("lights_on",40,20,30),
        ("lights_dim",50,-40,20),("lights_off",-10,-70,-10),("high_energy_use",-30,50,20),("low_energy_use",50,-20,30),
        ("eco_mode",60,-30,40),("music_playing",70,40,20),("tv_on",40,10,10),("cooking_time",60,40,30),
        ("laundry_done",50,20,20),("humid",-30,10,-10),("dry_air",-20,10,0),("good_air",60,-10,30),
        ("poor_air",-50,40,-20),("window_open",50,20,10),("window_closed",30,-20,20),("washer_running",30,10,10),
        ("alarm_armed",40,20,60),("alarm_triggered",-80,100,-60),("garage_open",10,40,-20),("garage_closed",40,-20,40),
        ("heating_on",40,10,20),("cooling_on",40,10,20),("vacuum_running",20,30,10),("guest_wifi",40,20,10),
        ("battery_low",-50,-30,-40),("battery_full",60,10,40),("internet_down",-60,70,-30),("update_ready",30,30,0),
        ("mail_arrived",50,50,10),("package_here",80,70,20),("sprinklers_on",30,10,10),("smoke_detected",-90,100,-70),
        ("slightly_hungry",-10,20,-10),("hungry",-30,40,-20),("starving",-70,70,-40),("just_fed",80,20,30),
        ("full",60,-30,20),("slightly_tired",-10,-30,-10),("tired",-30,-60,-30),("exhausted",-60,-80,-50),
        ("well_rested",70,40,40),("waking_up",20,10,-20),("slightly_bored",-20,-30,0),("very_bored",-50,-70,-10),
        ("entertained",60,40,20),("playful",70,70,30),("lonely",-60,-30,-50),("needy",-20,30,-50),
        ("overstimulated",-20,80,-10),("calm",50,-50,20),("zen",60,-80,30),("thirsty",-20,30,-10),
        ("refreshed",60,30,30),("attention_seek",10,60,-40),("grateful",70,20,10),("pouty",-40,20,-30),
        ("hyper",60,100,30),("snuggly",70,-40,-20),("grumpy",-50,30,20),("mellow",40,-40,10),
        ("frisky",50,70,30),("napping",30,-90,-30),
        ("doorbell",0,90,-30),("someone_arrived",60,70,10),("someone_left",-30,20,-10),("owner_home",90,60,20),
        ("owner_away",-50,-20,-40),("everyone_asleep",20,-80,10),("party_mode",80,90,40),("energy_spike",-20,60,20),
        ("rain_detected",30,-10,0),("storm_warning",-30,60,-30),("sunny",60,30,20),("cloudy",10,-20,0),
        ("snowing",40,10,0),("windy",10,30,-10),("motion_detected",-10,70,-20),("phone_ringing",10,50,-10),
        ("timer_done",40,50,20),("calendar_event",20,30,10),("trash_day",-10,10,10),("water_leak",-80,100,-60),
        ("co2_high",-40,50,-20),("noise_alert",-20,70,-20),("sunrise",50,20,10),("sunset",50,-20,10),
        ("power_outage",-60,80,-50),("backup_power",-20,40,10),("device_offline",-30,30,-10),("new_device",40,40,10),
        ("being_petted",85,20,0),("treat_received",90,50,20),("scolded",-60,30,-60),("praised",80,40,30),
        ("stranger_detect",-30,80,-50),("pet_detected",60,50,10),("car_arriving",40,50,10),("car_leaving",-20,20,-10),
        ("garden_dry",-20,20,0),("plant_happy",50,10,20),("fridge_open",10,40,-10),("goodnight",40,-60,0),
        ("dawn",40,-30,0),("early_morning",30,10,-10),("morning_coffee",50,20,20),("midmorning",50,30,30),
        ("noon",40,40,30),("afternoon_slump",-20,-40,-10),("late_afternoon",30,10,10),("golden_hour",70,-10,20),
        ("dusk",40,-30,0),("twilight",30,-40,-10),("early_evening",50,10,20),("dinner_time",60,30,20),
        ("relaxed_evening",60,-30,20),("movie_night",50,-20,10),("late_night",-10,-50,-10),("midnight",-10,-60,0),
        ("deep_night",0,-80,0),("witching_hour",-10,-40,10),("pre_dawn",10,-50,0),("weekend_morning",60,-10,20),
        ("sunday_lazy",50,-50,10),("monday_blues",-30,-20,-10),("friday_vibes",70,60,30),("lunch_time",50,30,20),
        ("naptime",30,-80,-20),("workout_time",50,70,50),("bedtime",20,-60,-10),("wee_hours",-10,-80,0),
        ("siesta",30,-70,-10),("tea_time",60,-20,20),
        ("christmas",90,60,30),("halloween",40,60,20),("valentines",90,40,10),("new_years",80,90,40),
        ("birthday",95,80,30),("fireworks",60,90,10),("spring_bloom",60,30,10),("summer_heat",-10,40,0),
        ("autumn_cozy",60,-20,20),("winter_cold",-20,10,-10),("aurora",70,10,10),("rainbow",80,40,10),
        ("meditation",50,-90,30),("gaming",60,70,40),("reading",50,-30,20),("working",20,40,50),
        ("creative_flow",70,50,60),("spa_day",70,-50,10),("dance_party",80,95,30),("candlelight",60,-40,10),
        ("stargazing",50,-30,0),("ocean_vibes",50,-20,0),("forest_bath",60,-40,10),("campfire",60,-10,20),
        ("thunderstorm",-20,70,-20),("snowfall",40,-20,0),("wind_howling",-10,40,-10),("heatwave",-40,30,-10),
        ("earthquake",-70,100,-70),("good_morning",60,30,20),("goodnight_kiss",80,-30,0),("tax_season",-40,40,20),
        ("payday",80,50,40),("cleaning_day",20,30,20),("baking",70,30,20),("garden_time",60,20,20),
        ("road_trip",70,60,30),("sick_day",-60,-40,-40),("celebration",90,80,30),("mystery",10,20,-20),
    ]

    # Render grids of 20 moods each (10 grids)
    COLS = 5
    ROWS = 4
    PER_GRID = COLS * ROWS  # 20

    for batch in range(0, len(ALL_MOODS), PER_GRID):
        chunk = ALL_MOODS[batch:batch + PER_GRID]
        grid_w = COLS * SIZE
        grid_h = ROWS * SIZE
        grid = Image.new('RGB', (grid_w, grid_h), (0, 0, 0))
        label_draw = ImageDraw.Draw(grid)

        for i, (name, p100, a100, d100) in enumerate(chunk):
            p, a, d = p100/100, a100/100, d100/100
            img = render_eye(name, p, a, d)
            col = i % COLS
            row = i // COLS
            grid.paste(img, (col * SIZE, row * SIZE))
            idx = batch + i
            tx = col * SIZE + SIZE // 2
            ty = row * SIZE + SIZE - 20
            label_draw.text((tx, ty), f"#{idx} {name}", fill=(255, 255, 255), anchor="ms")

        grid_num = batch // PER_GRID
        grid_path = os.path.join(OUT_DIR, f'batch_{grid_num:02d}_{batch}-{batch+len(chunk)-1}.png')
        grid.save(grid_path)
        print(f'  Batch {grid_num}: #{batch}-{batch+len(chunk)-1} -> {grid_path}')


if __name__ == '__main__':
    main()
