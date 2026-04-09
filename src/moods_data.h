#ifndef MOODS_DATA_H
#define MOODS_DATA_H

#include "moods.h"

// RGB565 color helpers
#define C_WHITE    0xFFFF
#define C_BLACK    0x0000
#define C_RED      RGB565(255, 0, 0)
#define C_ORANGE   RGB565(255, 140, 0)
#define C_YELLOW   RGB565(255, 220, 0)
#define C_GREEN    RGB565(0, 200, 0)
#define C_CYAN     RGB565(0, 200, 200)
#define C_BLUE     RGB565(0, 80, 255)
#define C_PURPLE   RGB565(140, 40, 255)
#define C_PINK     RGB565(255, 80, 180)
#define C_BRONZE   RGB565(205, 127, 50)
#define C_GOLD     RGB565(255, 200, 40)
#define C_TEAL     RGB565(0, 160, 140)
#define C_LAVENDER RGB565(180, 150, 255)
#define C_CORAL    RGB565(255, 110, 80)
#define C_MINT     RGB565(100, 255, 180)
#define C_DKBLUE   RGB565(0, 0, 80)
#define C_DKRED    RGB565(100, 0, 0)
#define C_DKGREEN  RGB565(0, 80, 0)
#define C_DKPURPLE RGB565(60, 0, 80)
#define C_WARM     RGB565(60, 30, 10)
#define C_COOL     RGB565(10, 20, 50)
#define C_NIGHT    RGB565(5, 5, 20)
#define C_SUNSET   RGB565(80, 30, 10)
#define C_DAWN     RGB565(40, 30, 50)
#define C_STORM    RGB565(30, 30, 40)

// name                         ew  eh  br  op  ps  px  py  lt  lb  la  eye_col    bg_col     glow_col   gr  bi  bv  ss  id  ba  bs
const MoodParams MOODS[NUM_MOODS] = {
  // ===== BASIC EMOTIONS (0-19) =====
  /* 0  */ {"happy",             140,140, 70,100, 35,  0,  0,  0, 25, 0,  C_BRONZE,  C_BLACK,   C_GOLD,    40,  4,  2,  3,  3,  3,  4},
  /* 1  */ {"sad",               120,100, 50, 80, 40,  0, 10, 20,  0, 15, C_BLUE,    C_BLACK,   C_DKBLUE,  30,  8,  3,  1,  1,  2,  2},
  /* 2  */ {"angry",             150,120, 40,100, 30,  0, -5, 10,  0,-25, C_RED,     C_DKRED,   C_RED,     45,  2,  1,  7,  2,  5,  7},
  /* 3  */ {"scared",            160,170, 80,100, 45,  0,  0,  0,  0,  0, C_WHITE,   C_BLACK,   C_PURPLE,  50, 10,  4,  8,  6,  6,  8},
  /* 4  */ {"surprised",         170,180, 85,100, 50,  0, -5,  0,  0,  0, C_WHITE,   C_BLACK,   C_YELLOW,  55,  6,  2,  5,  4,  4,  6},
  /* 5  */ {"content",           130,130, 65, 90, 35,  0,  5,  5, 15,  5, C_BRONZE,  C_BLACK,   C_WARM,    35,  5,  3,  2,  2,  3,  3},
  /* 6  */ {"excited",           160,160, 80,100, 40,  0,  0,  0, 20,  0, C_GOLD,    C_BLACK,   C_YELLOW,  50,  2,  1,  6,  5,  7,  8},
  /* 7  */ {"bored",             110, 80, 40, 60, 30,  0, 15, 30,  0, 20, C_BRONZE,  C_BLACK,   C_BLACK,    0, 10,  5,  0,  0,  1,  1},
  /* 8  */ {"curious",           150,160, 75,100, 45, 20, -5,  0,  0,  0, C_TEAL,    C_BLACK,   C_CYAN,    40,  5,  2,  4,  5,  2,  3},
  /* 9  */ {"love",              140,140, 70,100, 35,  0,  5,  0, 20,  5, C_PINK,    C_BLACK,   C_PINK,    45,  4,  2,  2,  3,  4,  4},
  /* 10 */ {"disgusted",         120,100, 45, 70, 25,  0,  5, 15,  0,-15, C_GREEN,   C_DKGREEN, C_GREEN,   20,  6,  3,  3,  2,  1,  2},
  /* 11 */ {"jealous",           130,110, 50, 85, 30, 15,  0, 10,  0,-10, C_GREEN,   C_BLACK,   C_DKGREEN, 25,  4,  2,  5,  3,  2,  3},
  /* 12 */ {"proud",             150,150, 75,100, 35,  0, -5,  0, 10, -5, C_GOLD,    C_BLACK,   C_GOLD,    45,  5,  2,  2,  2,  3,  3},
  /* 13 */ {"guilty",            110,100, 50, 70, 30,  0, 20, 15,  0, 10, C_BRONZE,  C_BLACK,   C_DKPURPLE,20,  8,  4,  1,  1,  1,  1},
  /* 14 */ {"hopeful",           140,150, 70, 95, 40,  0, -5,  0,  5,  0, C_LAVENDER,C_BLACK,   C_CYAN,    40,  4,  2,  3,  3,  3,  4},
  /* 15 */ {"nervous",           130,140, 60, 90, 35,  0,  0,  5,  0,  5, C_YELLOW,  C_BLACK,   C_YELLOW,  30,  3,  2,  8,  4,  5,  6},
  /* 16 */ {"peaceful",          120,120, 60, 80, 35,  0,  5, 10, 15, 10, C_CYAN,    C_COOL,    C_TEAL,    35,  7,  3,  1,  1,  2,  2},
  /* 17 */ {"mischievous",       140,120, 55, 90, 30, 15, -5,  5, 10,-10, C_ORANGE,  C_BLACK,   C_ORANGE,  35,  3,  2,  6,  5,  4,  5},
  /* 18 */ {"confused",          140,150, 65, 95, 40,-10,  5,  5,  0,  8, C_LAVENDER,C_BLACK,   C_PURPLE,  30,  4,  2,  7,  6,  3,  4},
  /* 19 */ {"determined",        140,130, 55,100, 30,  0, -5,  5,  0,-15, C_BRONZE,  C_BLACK,   C_ORANGE,  35,  5,  2,  2,  2,  2,  3},

  // ===== HOME STATES (20-59) =====
  /* 20 */ {"cozy_evening",      120,120, 60, 80, 35,  0,  5, 10, 15, 10, C_ORANGE,  C_WARM,    C_ORANGE,  40,  6,  3,  1,  1,  2,  2},
  /* 21 */ {"morning_energy",    150,150, 75,100, 35,  0, -5,  0, 10,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  45,  4,  2,  4,  4,  4,  5},
  /* 22 */ {"too_hot",           140,120, 55, 85, 30,  0,  5, 15,  0, 10, C_RED,     C_DKRED,   C_ORANGE,  35,  3,  2,  5,  3,  3,  4},
  /* 23 */ {"too_cold",          100, 90, 45, 70, 25,  0,  5, 20,  0, 15, C_BLUE,    C_DKBLUE,  C_CYAN,    25,  8,  3,  6,  2,  7,  8},
  /* 24 */ {"perfect_temp",      130,130, 65, 90, 35,  0,  0,  0, 15,  5, C_MINT,    C_BLACK,   C_MINT,    40,  5,  3,  2,  2,  2,  3},
  /* 25 */ {"door_unlocked",     150,160, 70,100, 40,  0, -5,  5,  0, -5, C_YELLOW,  C_BLACK,   C_YELLOW,  45,  3,  1,  7,  5,  4,  5},
  /* 26 */ {"door_locked",       120,120, 60, 85, 35,  0,  5,  5, 15, 10, C_GREEN,   C_BLACK,   C_GREEN,   30,  6,  3,  1,  1,  2,  2},
  /* 27 */ {"lights_on",         140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  50,  4,  2,  3,  3,  2,  3},
  /* 28 */ {"lights_dim",        110,110, 55, 70, 30,  0,  5, 15, 10, 10, C_ORANGE,  C_WARM,    C_ORANGE,  25,  7,  3,  1,  1,  2,  2},
  /* 29 */ {"lights_off",        100, 80, 40, 50, 25,  0, 10, 25,  0, 15, C_DKBLUE,  C_NIGHT,   C_DKBLUE,  15, 10,  4,  0,  0,  1,  1},
  /* 30 */ {"high_energy_use",   140,130, 55, 90, 30,  0, -5, 10,  0,-10, C_RED,     C_BLACK,   C_RED,     35,  3,  2,  6,  4,  4,  5},
  /* 31 */ {"low_energy_use",    130,130, 65, 90, 35,  0,  5,  0, 15,  5, C_GREEN,   C_BLACK,   C_GREEN,   35,  5,  3,  2,  2,  2,  3},
  /* 32 */ {"eco_mode",          120,120, 60, 85, 35,  0,  0,  5, 10,  5, C_MINT,    C_DKGREEN, C_MINT,    30,  6,  3,  1,  1,  2,  2},
  /* 33 */ {"music_playing",     140,140, 70, 95, 35,  0,  0,  0, 15,  0, C_PURPLE,  C_DKPURPLE,C_LAVENDER,40,  4,  2,  3,  3,  5,  6},
  /* 34 */ {"tv_on",             130,120, 55, 80, 35,  5,  0, 10,  5,  5, C_CYAN,    C_COOL,    C_CYAN,    30,  5,  3,  2,  2,  1,  2},
  /* 35 */ {"cooking_time",      140,140, 65, 95, 35,  0,  0,  5, 10,  0, C_ORANGE,  C_WARM,    C_ORANGE,  40,  4,  2,  4,  3,  3,  4},
  /* 36 */ {"laundry_done",      130,130, 65, 90, 35,  0,  0,  0, 10,  0, C_CYAN,    C_BLACK,   C_CYAN,    35,  4,  2,  3,  3,  3,  4},
  /* 37 */ {"humid",             120,110, 50, 75, 30,  0, 10, 20,  0, 15, C_TEAL,    C_COOL,    C_TEAL,    25,  7,  3,  2,  1,  2,  2},
  /* 38 */ {"dry_air",           120,110, 50, 80, 30,  0,  0, 10,  0,  5, C_YELLOW,  C_BLACK,   C_YELLOW,  25,  5,  3,  3,  2,  2,  3},
  /* 39 */ {"good_air_quality",  130,130, 65, 90, 35,  0,  0,  0, 10,  5, C_MINT,    C_BLACK,   C_MINT,    35,  5,  3,  2,  2,  3,  3},
  /* 40 */ {"poor_air_quality",  110,100, 45, 70, 25,  0,  5, 15,  0, 10, C_YELLOW,  C_BLACK,   C_DKGREEN, 20,  6,  3,  4,  2,  3,  4},
  /* 41 */ {"window_open",       140,150, 70, 95, 40,  0, -5,  0,  5,  0, C_CYAN,    C_BLACK,   C_MINT,    40,  4,  2,  3,  4,  3,  4},
  /* 42 */ {"window_closed",     120,120, 60, 85, 35,  0,  5,  5, 10, 10, C_BRONZE,  C_BLACK,   C_WARM,    30,  6,  3,  1,  1,  2,  2},
  /* 43 */ {"washer_running",    130,130, 60, 85, 35,  0,  0,  5,  5,  0, C_CYAN,    C_BLACK,   C_CYAN,    30,  5,  2,  3,  2,  6,  5},
  /* 44 */ {"alarm_armed",       130,140, 60, 95, 30,  0, -5,  5,  0, -5, C_GREEN,   C_BLACK,   C_GREEN,   35,  5,  2,  3,  3,  2,  3},
  /* 45 */ {"alarm_triggered",   170,180, 85,100, 50,  0,  0,  0,  0,  0, C_RED,     C_DKRED,   C_RED,     55,  1,  0, 10,  8,  8, 10},
  /* 46 */ {"garage_open",       140,150, 65,100, 40, 10, -5,  5,  0, -5, C_YELLOW,  C_BLACK,   C_YELLOW,  40,  3,  2,  6,  5,  3,  4},
  /* 47 */ {"garage_closed",     120,120, 60, 85, 35,  0,  5,  5, 10, 10, C_GREEN,   C_BLACK,   C_GREEN,   30,  6,  3,  1,  1,  2,  2},
  /* 48 */ {"thermostat_heat",   130,130, 60, 85, 35,  0,  0,  5,  5,  0, C_ORANGE,  C_WARM,    C_RED,     35,  5,  3,  2,  2,  3,  3},
  /* 49 */ {"thermostat_cool",   130,130, 60, 85, 35,  0,  0,  5,  5,  0, C_CYAN,    C_COOL,    C_BLUE,    35,  5,  3,  2,  2,  3,  3},
  /* 50 */ {"vacuum_running",    130,120, 55, 85, 30,  5,  0, 10,  0,  0, C_TEAL,    C_BLACK,   C_TEAL,    30,  4,  2,  5,  4,  4,  5},
  /* 51 */ {"guest_wifi",        140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_PURPLE,  C_BLACK,   C_LAVENDER,40,  4,  2,  3,  3,  3,  3},
  /* 52 */ {"battery_low",       110,100, 45, 65, 25,  0, 10, 25,  0, 15, C_RED,     C_BLACK,   C_DKRED,   15,  8,  4,  2,  1,  1,  1},
  /* 53 */ {"battery_full",      140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_GREEN,   C_BLACK,   C_GREEN,   40,  5,  2,  3,  3,  3,  4},
  /* 54 */ {"internet_down",     130,140, 55, 90, 35,  0,  0,  5,  0, -5, C_RED,     C_BLACK,   C_RED,     30,  3,  2,  8,  5,  5,  6},
  /* 55 */ {"update_available",  140,150, 70, 95, 40,  0, -5,  0,  5,  0, C_CYAN,    C_BLACK,   C_CYAN,    40,  4,  2,  4,  4,  3,  4},
  /* 56 */ {"mail_arrived",      140,150, 70,100, 40,  0, -5,  0,  5,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  40,  3,  2,  5,  4,  4,  5},
  /* 57 */ {"package_delivered",  160,160, 80,100, 45,  0,  0,  0, 15,  0, C_GOLD,    C_BLACK,   C_GOLD,    50,  3,  1,  5,  5,  5,  6},
  /* 58 */ {"sprinklers_on",     130,130, 65, 90, 35,  0,  0,  0, 10,  0, C_CYAN,    C_COOL,    C_BLUE,    35,  5,  2,  3,  3,  3,  4},
  /* 59 */ {"smoke_detected",    170,180, 85,100, 50,  0,  0,  0,  0,  0, C_RED,     C_DKRED,   C_ORANGE,  55,  1,  0, 10, 10,  9, 10},

  // ===== PET NEEDS (60-89) =====
  /* 60 */ {"slightly_hungry",   130,120, 55, 85, 30,  0,  5, 10,  0,  5, C_ORANGE,  C_BLACK,   C_ORANGE,  30,  5,  2,  3,  3,  2,  3},
  /* 61 */ {"hungry",            140,130, 55, 90, 35,  0,  0, 10,  0,  0, C_ORANGE,  C_BLACK,   C_ORANGE,  35,  4,  2,  5,  4,  4,  5},
  /* 62 */ {"starving",          150,140, 60,100, 40,  0, -5,  5,  0,-10, C_RED,     C_DKRED,   C_ORANGE,  40,  2,  1,  7,  5,  6,  7},
  /* 63 */ {"just_fed",          130,130, 65, 90, 35,  0,  5,  0, 20,  5, C_GOLD,    C_BLACK,   C_GOLD,    40,  5,  3,  2,  2,  3,  3},
  /* 64 */ {"full",              120,110, 55, 80, 30,  0, 10, 15, 15, 10, C_BRONZE,  C_WARM,    C_ORANGE,  30,  7,  3,  1,  1,  2,  2},
  /* 65 */ {"slightly_tired",    120,110, 55, 75, 30,  0,  5, 15, 10, 10, C_LAVENDER,C_BLACK,   C_DKPURPLE,25,  6,  3,  2,  1,  2,  2},
  /* 66 */ {"tired",             110,100, 50, 60, 25,  0, 10, 25,  5, 20, C_LAVENDER,C_NIGHT,   C_DKPURPLE,20,  8,  4,  1,  1,  1,  1},
  /* 67 */ {"exhausted",         100, 80, 40, 40, 20,  0, 15, 35,  0, 25, C_DKBLUE,  C_NIGHT,   C_DKBLUE,  10, 12,  5,  0,  0,  1,  1},
  /* 68 */ {"well_rested",       150,150, 75,100, 35,  0, -5,  0, 10,  0, C_MINT,    C_BLACK,   C_CYAN,    45,  4,  2,  4,  4,  4,  5},
  /* 69 */ {"waking_up",         120,130, 60, 70, 30,  0,  5, 20,  5, 10, C_YELLOW,  C_WARM,    C_ORANGE,  30,  5,  3,  3,  2,  3,  3},
  /* 70 */ {"slightly_bored",    120,100, 50, 75, 30, 10, 10, 20,  0, 10, C_BRONZE,  C_BLACK,   C_BLACK,    0,  7,  4,  2,  3,  1,  1},
  /* 71 */ {"very_bored",        110, 80, 40, 55, 25,-15, 15, 30,  0, 20, C_BRONZE,  C_BLACK,   C_BLACK,    0, 10,  5,  1,  1,  0,  1},
  /* 72 */ {"entertained",       140,140, 70, 95, 35,  5,  0,  0, 15,  0, C_GOLD,    C_BLACK,   C_YELLOW,  40,  4,  2,  4,  4,  4,  5},
  /* 73 */ {"playful",           150,150, 75,100, 40,  0, -5,  0, 10,  0, C_PINK,    C_BLACK,   C_PINK,    45,  3,  2,  6,  6,  6,  7},
  /* 74 */ {"lonely",            110,100, 50, 70, 35,  0, 15, 15,  0, 10, C_BLUE,    C_DKBLUE,  C_BLUE,    25,  8,  3,  1,  1,  2,  2},
  /* 75 */ {"needy",             130,140, 65, 95, 45,  0, -5,  0,  0,  5, C_PINK,    C_BLACK,   C_PINK,    40,  3,  2,  5,  5,  4,  5},
  /* 76 */ {"overstimulated",    140,130, 55, 90, 30,  0,  0,  5,  0, -5, C_YELLOW,  C_BLACK,   C_YELLOW,  35,  2,  1,  9,  7,  6,  8},
  /* 77 */ {"calm",              120,120, 60, 85, 35,  0,  5,  5, 10,  5, C_CYAN,    C_COOL,    C_TEAL,    30,  6,  3,  1,  1,  2,  2},
  /* 78 */ {"zen",               110,110, 55, 75, 30,  0,  5, 10, 15, 10, C_TEAL,    C_COOL,    C_TEAL,    25,  8,  3,  0,  0,  2,  1},
  /* 79 */ {"thirsty",           130,120, 55, 85, 30,  0,  0, 10,  0,  0, C_CYAN,    C_BLACK,   C_BLUE,    30,  4,  2,  4,  3,  3,  4},
  /* 80 */ {"refreshed",         140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_CYAN,    C_BLACK,   C_MINT,    40,  4,  2,  3,  3,  3,  4},
  /* 81 */ {"attention_seeking",  150,160, 75,100, 45,  0, -5,  0,  5,  0, C_GOLD,    C_BLACK,   C_YELLOW,  50,  2,  1,  6,  6,  5,  6},
  /* 82 */ {"grateful",          130,130, 65, 90, 35,  0,  5,  0, 20,  5, C_GOLD,    C_WARM,    C_GOLD,    35,  5,  3,  2,  2,  3,  3},
  /* 83 */ {"pouty",             110, 90, 45, 70, 25,  0, 10, 10, 30, 15, C_BLUE,    C_BLACK,   C_DKBLUE,  20,  6,  3,  2,  2,  2,  2},
  /* 84 */ {"hyper",             160,160, 80,100, 40,  0,  0,  0, 10,  0, C_YELLOW,  C_BLACK,   C_GOLD,    50,  2,  1,  8,  8,  8,  9},
  /* 85 */ {"snuggly",           110,110, 55, 75, 30,  0, 10, 15, 20, 15, C_PINK,    C_WARM,    C_PINK,    25,  7,  3,  1,  1,  2,  2},
  /* 86 */ {"grumpy",            120,100, 45, 75, 25,  0,  5, 15,  0,-20, C_ORANGE,  C_BLACK,   C_DKRED,   20,  6,  3,  3,  2,  2,  3},
  /* 87 */ {"mellow",            120,110, 55, 80, 30,  0,  5, 10, 10, 10, C_LAVENDER,C_DKPURPLE,C_LAVENDER,25,  6,  3,  1,  1,  2,  2},
  /* 88 */ {"frisky",            150,140, 65,100, 35,  5, -5,  0,  5, -5, C_CORAL,   C_BLACK,   C_ORANGE,  40,  3,  2,  7,  6,  5,  6},
  /* 89 */ {"napping",            90, 60, 30, 20, 20,  0, 15, 40,  0, 25, C_LAVENDER,C_NIGHT,   C_DKPURPLE,10, 15,  5,  0,  0,  1,  1},

  // ===== REACTIVE (90-129) =====
  /* 90 */ {"doorbell",          170,180, 85,100, 50,  0, -5,  0,  0,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  55,  2,  1,  7,  6,  5,  7},
  /* 91 */ {"someone_arrived",   160,170, 80,100, 45,  0, -5,  0, 10,  0, C_GOLD,    C_BLACK,   C_GOLD,    50,  3,  1,  5,  5,  5,  6},
  /* 92 */ {"someone_left",      120,110, 55, 80, 30,  0, 10, 10,  0,  5, C_BLUE,    C_BLACK,   C_BLUE,    25,  7,  3,  2,  2,  2,  2},
  /* 93 */ {"owner_home",        150,150, 75,100, 40,  0,  0,  0, 20,  0, C_PINK,    C_BLACK,   C_PINK,    45,  3,  2,  4,  4,  5,  5},
  /* 94 */ {"owner_away",        110,100, 50, 70, 35,  0, 15, 15,  0, 10, C_BLUE,    C_COOL,    C_DKBLUE,  25,  8,  3,  1,  1,  2,  2},
  /* 95 */ {"everyone_asleep",   100, 80, 40, 40, 25,  0, 15, 30,  0, 20, C_DKBLUE,  C_NIGHT,   C_DKBLUE,  15, 12,  5,  0,  0,  1,  1},
  /* 96 */ {"party_mode",        160,160, 80,100, 40,  0,  0,  0, 15,  0, C_PURPLE,  C_DKPURPLE,C_PINK,    50,  2,  1,  7,  7,  7,  8},
  /* 97 */ {"energy_spike",      140,140, 60, 95, 35,  0, -5, 10,  0,-10, C_YELLOW,  C_BLACK,   C_RED,     40,  3,  2,  6,  4,  5,  6},
  /* 98 */ {"rain_detected",     120,120, 60, 85, 35,  0,  5,  5, 10,  5, C_BLUE,    C_COOL,    C_CYAN,    30,  5,  3,  2,  2,  3,  3},
  /* 99 */ {"storm_warning",     140,150, 65, 95, 35,  0,  0, 10,  0, -5, C_PURPLE,  C_STORM,   C_PURPLE,  40,  3,  2,  7,  5,  5,  6},
  /*100 */ {"sunny",             140,140, 70, 95, 35,  0,  0,  0, 15,  5, C_YELLOW,  C_BLACK,   C_GOLD,    40,  5,  2,  3,  3,  3,  4},
  /*101 */ {"cloudy",            120,110, 55, 80, 30,  0,  5, 10,  5,  5, C_LAVENDER,C_COOL,    C_LAVENDER,25,  6,  3,  2,  2,  2,  2},
  /*102 */ {"snowing",           130,130, 65, 85, 35,  0,  0,  5,  5,  0, C_WHITE,   C_COOL,    C_CYAN,    35,  5,  3,  3,  3,  3,  3},
  /*103 */ {"windy",             130,120, 55, 85, 30,  5,  0,  5,  0,  0, C_CYAN,    C_BLACK,   C_TEAL,    30,  4,  2,  5,  4,  4,  5},
  /*104 */ {"motion_detected",   150,160, 75,100, 45, 15, -5,  0,  0,  0, C_WHITE,   C_BLACK,   C_YELLOW,  45,  3,  2,  6,  6,  4,  5},
  /*105 */ {"phone_ringing",     140,150, 70,100, 40,  0,  0,  0,  5,  0, C_CYAN,    C_BLACK,   C_CYAN,    40,  3,  1,  5,  5,  5,  6},
  /*106 */ {"timer_done",        150,150, 75,100, 40,  0, -5,  0, 10,  0, C_GREEN,   C_BLACK,   C_GREEN,   45,  3,  2,  5,  4,  4,  5},
  /*107 */ {"calendar_event",    140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_PURPLE,  C_BLACK,   C_LAVENDER,40,  4,  2,  3,  3,  3,  4},
  /*108 */ {"trash_day",         120,110, 50, 80, 30,  0,  0, 10,  0,  0, C_GREEN,   C_DKGREEN, C_GREEN,   25,  5,  3,  3,  3,  2,  3},
  /*109 */ {"water_leak",        160,170, 80,100, 50,  0,  0,  0,  0,  0, C_CYAN,    C_BLACK,   C_BLUE,    55,  2,  0, 10,  8,  8, 10},
  /*110 */ {"co2_high",          140,130, 55, 90, 30,  0,  5, 10,  0, -5, C_YELLOW,  C_BLACK,   C_ORANGE,  35,  3,  2,  5,  3,  4,  5},
  /*111 */ {"noise_alert",       150,160, 75,100, 45,  0, -5,  5,  0, -5, C_ORANGE,  C_BLACK,   C_ORANGE,  45,  2,  1,  8,  6,  5,  7},
  /*112 */ {"sunrise",           140,140, 70, 90, 35,  0,  0,  0, 10,  5, C_GOLD,    C_WARM,    C_YELLOW,  40,  5,  2,  2,  2,  3,  3},
  /*113 */ {"sunset",            120,120, 60, 85, 35,  0,  5,  5, 10, 10, C_ORANGE,  C_SUNSET,  C_CORAL,   35,  6,  3,  1,  1,  2,  2},
  /*114 */ {"power_outage",      130,140, 60, 90, 35,  0,  0, 10,  0,-10, C_RED,     C_BLACK,   C_DKRED,   20,  3,  2,  8,  5,  6,  7},
  /*115 */ {"backup_power",      130,130, 60, 85, 30,  0,  5, 10,  0, -5, C_YELLOW,  C_BLACK,   C_ORANGE,  30,  4,  2,  4,  3,  3,  4},
  /*116 */ {"device_offline",    120,110, 50, 75, 30,  0,  5, 15,  0,  5, C_RED,     C_BLACK,   C_DKRED,   20,  6,  3,  3,  2,  2,  3},
  /*117 */ {"new_device",        140,150, 70,100, 40,  0, -5,  0,  5,  0, C_CYAN,    C_BLACK,   C_CYAN,    40,  3,  2,  5,  5,  4,  5},
  /*118 */ {"being_petted",      130,120, 60, 85, 35,  0,  5,  0, 25, 10, C_PINK,    C_BLACK,   C_PINK,    40,  5,  3,  2,  2,  3,  3},
  /*119 */ {"treat_received",    150,150, 75,100, 40,  0, -5,  0, 20,  0, C_GOLD,    C_BLACK,   C_GOLD,    50,  3,  1,  4,  4,  5,  6},
  /*120 */ {"scolded",           110, 90, 45, 65, 25,  0, 20, 20,  0, 15, C_BLUE,    C_BLACK,   C_DKBLUE,  15,  8,  4,  2,  1,  2,  2},
  /*121 */ {"praised",           150,150, 75,100, 40,  0, -5,  0, 15,  0, C_GOLD,    C_WARM,    C_GOLD,    50,  3,  2,  3,  3,  4,  5},
  /*122 */ {"stranger_detected",  160,170, 80,100, 45, 20, -5,  5,  0, -5, C_YELLOW,  C_BLACK,   C_ORANGE,  50,  2,  1,  7,  6,  5,  7},
  /*123 */ {"pet_detected",      150,150, 75,100, 40,  5, -5,  0, 10,  0, C_PINK,    C_BLACK,   C_PINK,    45,  3,  2,  5,  5,  4,  5},
  /*124 */ {"car_arriving",      140,150, 70,100, 40, 10, -5,  0,  5,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  40,  3,  2,  5,  5,  4,  5},
  /*125 */ {"car_leaving",       120,110, 55, 80, 30,  0, 10, 10,  0,  5, C_BLUE,    C_BLACK,   C_BLUE,    25,  7,  3,  2,  2,  2,  2},
  /*126 */ {"garden_needs_water", 130,120, 55, 85, 30,  0,  0, 10,  0,  5, C_YELLOW,  C_BLACK,   C_GREEN,   30,  5,  2,  3,  3,  2,  3},
  /*127 */ {"plant_happy",       130,130, 65, 90, 35,  0,  0,  0, 15,  5, C_GREEN,   C_DKGREEN, C_MINT,    35,  5,  3,  2,  2,  3,  3},
  /*128 */ {"fridge_open",       140,150, 70,100, 40,  5, -5,  5,  0, -5, C_CYAN,    C_BLACK,   C_WHITE,   40,  3,  2,  6,  5,  4,  5},
  /*129 */ {"goodnight",         100, 80, 40, 40, 25,  0, 15, 30, 10, 20, C_LAVENDER,C_NIGHT,   C_DKPURPLE,15, 10,  5,  0,  0,  2,  1},

  // ===== TIME-BASED (130-159) =====
  /*130 */ {"dawn_calm",         120,130, 60, 75, 30,  0,  5, 15, 10, 10, C_LAVENDER,C_DAWN,    C_PINK,    30,  6,  3,  1,  1,  2,  2},
  /*131 */ {"early_morning",     130,140, 65, 85, 35,  0,  0, 10,  5,  5, C_YELLOW,  C_WARM,    C_ORANGE,  35,  5,  3,  2,  2,  3,  3},
  /*132 */ {"morning_coffee",    140,140, 70, 90, 35,  0,  0,  5, 10,  0, C_BRONZE,  C_WARM,    C_ORANGE,  35,  5,  2,  3,  3,  3,  4},
  /*133 */ {"midmorning",        140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_YELLOW,  C_BLACK,   C_YELLOW,  40,  4,  2,  3,  3,  3,  4},
  /*134 */ {"noon",              150,150, 75,100, 35,  0, -5,  0,  5,  0, C_GOLD,    C_BLACK,   C_YELLOW,  45,  4,  2,  4,  4,  3,  4},
  /*135 */ {"afternoon_slump",   110,100, 50, 70, 30,  0, 10, 20,  0, 15, C_ORANGE,  C_WARM,    C_ORANGE,  25,  7,  3,  1,  1,  2,  2},
  /*136 */ {"late_afternoon",    130,130, 65, 85, 35,  0,  5,  5,  5,  5, C_ORANGE,  C_BLACK,   C_ORANGE,  30,  5,  3,  2,  2,  2,  3},
  /*137 */ {"golden_hour",       140,140, 70, 90, 35,  0,  0,  0, 15,  5, C_GOLD,    C_SUNSET,  C_GOLD,    40,  5,  3,  2,  2,  3,  3},
  /*138 */ {"dusk",              120,120, 60, 80, 30,  0,  5, 10, 10, 10, C_CORAL,   C_SUNSET,  C_PURPLE,  30,  6,  3,  1,  1,  2,  2},
  /*139 */ {"twilight",          110,110, 55, 75, 30,  0,  5, 15, 10, 10, C_PURPLE,  C_DKPURPLE,C_LAVENDER,25,  7,  3,  1,  1,  2,  2},
  /*140 */ {"early_evening",     130,130, 65, 85, 35,  0,  5,  5, 10,  5, C_ORANGE,  C_WARM,    C_ORANGE,  30,  5,  3,  2,  2,  2,  3},
  /*141 */ {"dinner_time",       140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_BRONZE,  C_WARM,    C_ORANGE,  35,  4,  2,  3,  3,  3,  4},
  /*142 */ {"relaxed_evening",   120,120, 60, 80, 35,  0,  5, 10, 15, 10, C_ORANGE,  C_WARM,    C_ORANGE,  30,  6,  3,  1,  1,  2,  2},
  /*143 */ {"movie_night",       120,110, 55, 75, 30,  5,  5, 10, 10,  5, C_PURPLE,  C_NIGHT,   C_DKPURPLE,25,  6,  3,  2,  2,  1,  2},
  /*144 */ {"late_night",        110,100, 50, 65, 25,  0, 10, 20,  5, 15, C_DKBLUE,  C_NIGHT,   C_DKBLUE,  20,  8,  4,  1,  1,  1,  1},
  /*145 */ {"midnight",          100, 90, 45, 55, 25,  0, 10, 25,  0, 20, C_DKBLUE,  C_NIGHT,   C_DKPURPLE,15, 10,  5,  0,  0,  1,  1},
  /*146 */ {"deep_night",         90, 70, 35, 35, 20,  0, 15, 35,  0, 25, C_DKBLUE,  C_NIGHT,   C_DKBLUE,  10, 15,  5,  0,  0,  1,  1},
  /*147 */ {"witching_hour",     120,130, 60, 80, 30,  0,  0, 10,  0,  0, C_PURPLE,  C_NIGHT,   C_PURPLE,  30,  6,  3,  3,  3,  2,  3},
  /*148 */ {"pre_dawn",          110,120, 55, 70, 30,  0,  5, 15, 10, 10, C_LAVENDER,C_DAWN,    C_LAVENDER,25,  7,  3,  1,  1,  2,  2},
  /*149 */ {"weekend_morning",   130,130, 65, 80, 35,  0,  5, 10, 15, 10, C_YELLOW,  C_WARM,    C_YELLOW,  35,  6,  3,  2,  2,  3,  3},
  /*150 */ {"sunday_lazy",       110,100, 50, 70, 30,  0, 10, 20, 15, 15, C_BRONZE,  C_WARM,    C_ORANGE,  25,  8,  4,  1,  1,  2,  2},
  /*151 */ {"monday_blues",      120,100, 50, 70, 30,  0, 10, 15,  0, 10, C_BLUE,    C_COOL,    C_DKBLUE,  20,  7,  3,  2,  1,  2,  2},
  /*152 */ {"friday_vibes",      150,150, 75,100, 40,  0, -5,  0, 15,  0, C_GOLD,    C_BLACK,   C_GOLD,    45,  3,  2,  5,  5,  5,  6},
  /*153 */ {"lunch_time",        140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_ORANGE,  C_BLACK,   C_ORANGE,  35,  4,  2,  3,  3,  3,  4},
  /*154 */ {"naptime",           100, 80, 40, 35, 20,  0, 15, 35,  0, 22, C_LAVENDER,C_NIGHT,   C_DKPURPLE,12, 12,  5,  0,  0,  1,  1},
  /*155 */ {"workout_time",      150,150, 75,100, 35,  0, -5,  0,  5,  0, C_GREEN,   C_BLACK,   C_GREEN,   45,  3,  1,  5,  5,  5,  7},
  /*156 */ {"bedtime",           100, 90, 45, 50, 25,  0, 10, 25,  5, 20, C_LAVENDER,C_NIGHT,   C_DKPURPLE,15,  9,  4,  0,  0,  1,  1},
  /*157 */ {"wee_hours",          90, 70, 35, 30, 20,  0, 15, 38,  0, 25, C_DKBLUE,  C_NIGHT,   C_DKBLUE,   8, 15,  5,  0,  0,  1,  1},
  /*158 */ {"siesta",            100, 85, 40, 40, 22,  0, 15, 32,  5, 22, C_ORANGE,  C_WARM,    C_ORANGE,  12, 10,  5,  0,  0,  2,  1},
  /*159 */ {"tea_time",          130,130, 65, 85, 35,  0,  5,  5, 10,  5, C_BRONZE,  C_WARM,    C_ORANGE,  30,  5,  3,  2,  2,  2,  3},

  // ===== SPECIAL (160-199) =====
  /*160 */ {"christmas",         140,140, 70, 95, 35,  0,  0,  0, 15,  0, C_RED,     C_DKGREEN, C_RED,     40,  4,  2,  3,  3,  4,  4},
  /*161 */ {"halloween",         140,140, 70, 95, 35,  0,  0,  0, 10,  0, C_ORANGE,  C_BLACK,   C_ORANGE,  40,  4,  2,  4,  4,  4,  5},
  /*162 */ {"valentines",        140,140, 70, 95, 35,  0,  0,  0, 20,  5, C_PINK,    C_DKRED,   C_PINK,    45,  4,  2,  3,  3,  3,  4},
  /*163 */ {"new_years",         160,160, 80,100, 40,  0,  0,  0, 10,  0, C_GOLD,    C_BLACK,   C_GOLD,    55,  2,  1,  6,  6,  6,  7},
  /*164 */ {"birthday",          160,160, 80,100, 45,  0, -5,  0, 15,  0, C_PINK,    C_BLACK,   C_GOLD,    50,  3,  1,  5,  5,  6,  7},
  /*165 */ {"fireworks",         150,160, 75,100, 40,  0,  0,  0,  5,  0, C_WHITE,   C_NIGHT,   C_GOLD,    50,  2,  1,  7,  6,  6,  8},
  /*166 */ {"spring_bloom",      140,140, 70, 95, 35,  0,  0,  0, 10,  5, C_PINK,    C_DKGREEN, C_MINT,    40,  4,  2,  3,  3,  3,  4},
  /*167 */ {"summer_heat",       140,120, 55, 85, 30,  0,  5, 15,  0, 10, C_YELLOW,  C_WARM,    C_ORANGE,  35,  4,  2,  3,  3,  3,  4},
  /*168 */ {"autumn_cozy",       130,130, 65, 85, 35,  0,  5,  5, 15, 10, C_ORANGE,  C_WARM,    C_CORAL,   30,  5,  3,  2,  2,  2,  3},
  /*169 */ {"winter_cold",       110,100, 50, 75, 25,  0,  5, 15,  5, 10, C_CYAN,    C_COOL,    C_BLUE,    25,  7,  3,  3,  2,  5,  5},
  /*170 */ {"aurora",            130,140, 65, 90, 35,  0,  0,  0, 10,  0, C_GREEN,   C_NIGHT,   C_PURPLE,  40,  5,  3,  2,  2,  3,  3},
  /*171 */ {"rainbow",           140,140, 70, 95, 35,  0,  0,  0, 15,  0, C_CYAN,    C_BLACK,   C_PINK,    45,  4,  2,  3,  3,  4,  4},
  /*172 */ {"meditation",        110,110, 55, 70, 30,  0,  5, 15, 15, 10, C_TEAL,    C_COOL,    C_TEAL,    25,  8,  4,  0,  0,  3,  2},
  /*173 */ {"gaming",            150,150, 75,100, 40,  5, -5,  0,  5,  0, C_GREEN,   C_BLACK,   C_GREEN,   45,  3,  2,  6,  5,  4,  5},
  /*174 */ {"reading",           120,120, 60, 85, 35,  0,  5,  5, 10,  5, C_BRONZE,  C_WARM,    C_ORANGE,  30,  6,  3,  2,  2,  1,  2},
  /*175 */ {"working",           130,130, 60, 90, 30,  0,  0,  5,  0, -5, C_CYAN,    C_BLACK,   C_BLUE,    30,  4,  2,  3,  3,  2,  3},
  /*176 */ {"creative_flow",     140,150, 70, 95, 40,  0, -5,  0, 10,  0, C_PURPLE,  C_DKPURPLE,C_PINK,    40,  4,  2,  4,  4,  3,  4},
  /*177 */ {"spa_day",           120,120, 60, 80, 35,  0,  5, 10, 20, 10, C_MINT,    C_COOL,    C_TEAL,    30,  7,  3,  1,  1,  3,  2},
  /*178 */ {"dance_party",       150,150, 75,100, 40,  0,  0,  0, 10,  0, C_PINK,    C_DKPURPLE,C_PURPLE,  50,  2,  1,  7,  7,  8,  9},
  /*179 */ {"candlelight",       120,120, 60, 80, 30,  0,  5, 10, 10, 10, C_ORANGE,  C_WARM,    C_GOLD,    25,  6,  3,  2,  2,  3,  2},
  /*180 */ {"stargazing",        130,140, 65, 85, 35,  0, -5,  0,  5,  0, C_LAVENDER,C_NIGHT,   C_PURPLE,  35,  6,  3,  2,  2,  2,  2},
  /*181 */ {"ocean_vibes",       130,130, 65, 85, 35,  0,  0,  0, 10,  5, C_CYAN,    C_DKBLUE,  C_TEAL,    35,  5,  3,  2,  2,  4,  3},
  /*182 */ {"forest_bath",       130,130, 65, 85, 35,  0,  5,  5, 10,  5, C_GREEN,   C_DKGREEN, C_MINT,    30,  6,  3,  1,  1,  3,  2},
  /*183 */ {"campfire",          130,130, 60, 85, 30,  0,  5,  5, 10,  5, C_ORANGE,  C_WARM,    C_RED,     30,  5,  3,  2,  2,  3,  3},
  /*184 */ {"thunderstorm",      140,150, 65, 95, 35,  0,  0, 10,  0, -5, C_WHITE,   C_STORM,   C_PURPLE,  45,  3,  2,  6,  5,  5,  6},
  /*185 */ {"snowfall",          120,120, 60, 85, 35,  0,  5,  5, 10,  5, C_WHITE,   C_COOL,    C_CYAN,    30,  6,  3,  2,  2,  3,  2},
  /*186 */ {"wind_howling",      130,120, 55, 85, 30,  5,  0,  5,  0, -5, C_CYAN,    C_STORM,   C_TEAL,    30,  4,  2,  6,  5,  5,  6},
  /*187 */ {"heatwave",          130,110, 50, 75, 25,  0, 10, 20,  0, 15, C_RED,     C_DKRED,   C_ORANGE,  30,  5,  3,  3,  2,  3,  4},
  /*188 */ {"earthquake",        140,140, 60, 95, 35,  0,  0,  5,  0,  0, C_ORANGE,  C_BLACK,   C_RED,     35,  2,  1, 10, 10,  8, 10},
  /*189 */ {"good_morning",      140,150, 70, 95, 40,  0, -5,  0, 10,  0, C_GOLD,    C_WARM,    C_YELLOW,  40,  4,  2,  3,  3,  3,  4},
  /*190 */ {"goodnight_kiss",    110,110, 55, 75, 30,  0, 10, 10, 25, 15, C_PINK,    C_NIGHT,   C_PINK,    25,  7,  3,  1,  1,  2,  2},
  /*191 */ {"tax_season",        130,120, 50, 80, 25,  0,  5, 15,  0,-15, C_GREEN,   C_BLACK,   C_DKGREEN, 20,  5,  3,  5,  3,  3,  4},
  /*192 */ {"payday",            150,150, 75,100, 40,  0, -5,  0, 15,  0, C_GOLD,    C_BLACK,   C_GOLD,    50,  3,  2,  4,  4,  5,  6},
  /*193 */ {"cleaning_day",      130,130, 60, 85, 30,  0,  0,  5,  5, -5, C_CYAN,    C_BLACK,   C_TEAL,    30,  4,  2,  4,  4,  3,  4},
  /*194 */ {"baking",            140,140, 70, 95, 35,  0,  0,  0, 15,  0, C_ORANGE,  C_WARM,    C_GOLD,    35,  4,  2,  3,  3,  3,  4},
  /*195 */ {"garden_time",       140,140, 70, 95, 35,  0,  0,  0, 10,  5, C_GREEN,   C_DKGREEN, C_MINT,    40,  4,  2,  3,  3,  3,  4},
  /*196 */ {"road_trip",         150,150, 75,100, 40, 10, -5,  0,  5,  0, C_CYAN,    C_BLACK,   C_BLUE,    45,  4,  2,  5,  5,  4,  5},
  /*197 */ {"sick_day",          100, 90, 45, 60, 25,  0, 15, 25,  0, 18, C_GREEN,   C_BLACK,   C_DKGREEN, 15,  8,  4,  1,  1,  1,  1},
  /*198 */ {"celebration",       160,160, 80,100, 45,  0, -5,  0, 15,  0, C_GOLD,    C_BLACK,   C_PINK,    55,  2,  1,  5,  5,  7,  8},
  /*199 */ {"mystery",           130,140, 65, 90, 35,  0,  0, 10,  0,  0, C_PURPLE,  C_NIGHT,   C_PURPLE,  35,  5,  3,  3,  3,  3,  3},
};

#endif // MOODS_DATA_H
