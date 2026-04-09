#include "moods.h"

// 200 moods as PAD vectors (Pleasure, Arousal, Dominance) scaled -100 to 100
// The eye renderer maps these 3 values to all visual parameters procedurally.
//
// Reference emotion mapping:
//   Happy:     P+  A+  D+     Sad:      P-  A-  D-
//   Angry:     P-  A+  D+     Scared:   P-  A+  D-
//   Content:   P+  A-  D+     Bored:    P-  A-  D0
//   Excited:   P+  A++ D+     Calm:     P+  A-- D0
//   Surprised: P0  A++ D-     Disgusted: P-  A+  D+

//                                  name                 P    A    D
const Mood MOODS[NUM_MOODS] = {
  // ===== BASIC EMOTIONS (0-19) =====
  /*  0 */ {"happy",              80,  50,  50},
  /*  1 */ {"sad",               -70, -40, -50},
  /*  2 */ {"angry",             -60,  80,  70},
  /*  3 */ {"scared",            -70,  90, -80},
  /*  4 */ {"surprised",          10,  95, -40},
  /*  5 */ {"content",            70, -30,  40},
  /*  6 */ {"excited",            90,  90,  60},
  /*  7 */ {"bored",             -30, -60,   0},
  /*  8 */ {"curious",            40,  60, -30},   // more aroused, looking around
  /*  9 */ {"love",               95,  10, -10},   // soft, big pupils, not squinty
  /* 10 */ {"disgusted",         -70,  20,  60},   // less aroused than angry, more dominant
  /* 11 */ {"jealous",           -40,  40,  40},   // sideways energy, distinct from angry
  /* 12 */ {"proud",              60,  20,  90},   // very high dominance, upward
  /* 13 */ {"guilty",            -50, -30, -80},   // very low dominance, averted
  /* 14 */ {"hopeful",            60,  40, -20},   // looking up, slightly submissive
  /* 15 */ {"nervous",           -40,  75, -60},   // high arousal, very low dominance
  /* 16 */ {"peaceful",           60, -80,  20},   // even calmer than before
  /* 17 */ {"mischievous",        40,  65,  60},   // higher arousal+dominance
  /* 18 */ {"confused",          -20,  50, -50},   // more aroused, more submissive
  /* 19 */ {"determined",         40,  50,  90},   // max dominance

  // ===== HOME STATES (20-59) =====
  /* 20 */ {"cozy_evening",       75, -50,  30},   // calmer
  /* 21 */ {"morning_energy",     60,  65,  40},   // more energized
  /* 22 */ {"too_hot",           -40,  50, -10},
  /* 23 */ {"too_cold",          -50,  20, -20},   // lower pleasure, less aroused than scared
  /* 24 */ {"perfect_temp",       70, -20,  30},
  /* 25 */ {"door_unlocked",     -30,  75, -40},   // much more alert/worried
  /* 26 */ {"door_locked",        50, -40,  60},   // calmer, more secure feeling
  /* 27 */ {"lights_on",          50,  30,  30},   // slightly brighter
  /* 28 */ {"lights_dim",         50, -50,  20},   // dimmer = calmer
  /* 29 */ {"lights_off",        -10, -75, -10},   // very sleepy
  /* 30 */ {"high_energy_use",   -40,  55,  30},   // more worried, more dominant
  /* 31 */ {"low_energy_use",     55, -30,  40},   // slightly proud/eco
  /* 32 */ {"eco_mode",           65, -35,  50},   // content + proud
  /* 33 */ {"music_playing",      75,  50,  20},   // grooving
  /* 34 */ {"tv_on",              40, -10,  10},   // slightly zoned out
  /* 35 */ {"cooking_time",       65,  45,  40},   // active, engaged
  /* 36 */ {"laundry_done",       55,  30,  30},   // mild satisfaction
  /* 37 */ {"humid",             -35,  15, -20},   // slightly uncomfortable
  /* 38 */ {"dry_air",           -25, -10, -10},   // slightly irritated, flat
  /* 39 */ {"good_air",           60, -20,  30},
  /* 40 */ {"poor_air",          -55,  45, -20},   // more negative
  /* 41 */ {"window_open",        55,  25,  10},   // breezy, pleasant
  /* 42 */ {"window_closed",      30, -30,  20},   // sealed up, calm
  /* 43 */ {"washer_running",     25, -10,  10},   // ambient, slight zone-out
  /* 44 */ {"alarm_armed",        40,  30,  70},   // vigilant, dominant
  /* 45 */ {"alarm_triggered",   -80, 100, -60},
  /* 46 */ {"garage_open",        10,  55, -30},   // more alert
  /* 47 */ {"garage_closed",      45, -30,  50},   // secure
  /* 48 */ {"heating_on",         45, -15,  20},   // warm, mild
  /* 49 */ {"cooling_on",         40, -20,  15},   // cool, mild
  /* 50 */ {"vacuum_running",     15,  45,  10},   // busy, slightly annoyed
  /* 51 */ {"guest_wifi",         45,  35,  20},   // social, alert
  /* 52 */ {"battery_low",       -55, -35, -45},   // even more drained
  /* 53 */ {"battery_full",       65,  15,  45},   // charged up
  /* 54 */ {"internet_down",     -50,  55, -40},   // confused + frustrated, not angry
  /* 55 */ {"update_ready",       35,  40,   0},   // more curious
  /* 56 */ {"mail_arrived",       55,  60,  10},   // more excited
  /* 57 */ {"package_here",       85,  75,  20},   // very excited
  /* 58 */ {"sprinklers_on",      25, -15,  10},   // ambient
  /* 59 */ {"smoke_detected",    -90, 100, -70},

  // ===== PET NEEDS (60-89) =====
  /* 60 */ {"slightly_hungry",   -15,  25, -15},
  /* 61 */ {"hungry",            -40,  50, -30},   // more spread from slightly_hungry
  /* 62 */ {"starving",          -70,  75, -45},
  /* 63 */ {"just_fed",           85,  25,  30},
  /* 64 */ {"full",               55, -40,  20},   // very content, drowsy
  /* 65 */ {"slightly_tired",    -15, -40, -15},   // more spread from slightly_bored
  /* 66 */ {"tired",             -30, -60, -30},
  /* 67 */ {"exhausted",         -60, -85, -50},
  /* 68 */ {"well_rested",        75,  45,  45},
  /* 69 */ {"waking_up",          15,  20, -25},   // groggy but coming alive
  /* 70 */ {"slightly_bored",    -25, -45,   5},   // more spread from slightly_tired
  /* 71 */ {"very_bored",        -50, -70, -10},
  /* 72 */ {"entertained",        65,  45,  25},
  /* 73 */ {"playful",            75,  75,  30},
  /* 74 */ {"lonely",            -65, -35, -70},   // very low dominance, blue not angry
  /* 75 */ {"needy",             -25,  40, -60},   // more aroused, very submissive
  /* 76 */ {"overstimulated",    -35,  90, -15},   // much higher arousal, frazzled
  /* 77 */ {"calm",               55, -55,  25},
  /* 78 */ {"zen",                60, -85,  30},
  /* 79 */ {"thirsty",           -25,  35, -15},
  /* 80 */ {"refreshed",          65,  35,  35},
  /* 81 */ {"attention_seek",      5,  70, -50},   // more desperate
  /* 82 */ {"grateful",           75,  15,  10},   // calmer, warm
  /* 83 */ {"pouty",             -45,  25, -35},
  /* 84 */ {"hyper",              65, 100,  30},
  /* 85 */ {"snuggly",            75, -45, -10},   // warmer, content not sleepy
  /* 86 */ {"grumpy",            -50,  35,  25},
  /* 87 */ {"mellow",             45, -45,  10},
  /* 88 */ {"frisky",             55,  80,  35},   // more aroused, playful
  /* 89 */ {"napping",            30, -90, -30},

  // ===== REACTIVE (90-129) =====
  /* 90 */ {"doorbell",           -5,  95, -35},   // much more startled
  /* 91 */ {"someone_arrived",    65,  75,  15},   // more excited
  /* 92 */ {"someone_left",      -35,  15, -15},
  /* 93 */ {"owner_home",         95,  65,  20},
  /* 94 */ {"owner_away",        -55, -25, -45},
  /* 95 */ {"everyone_asleep",    20, -85,  10},
  /* 96 */ {"party_mode",         85,  95,  40},
  /* 97 */ {"energy_spike",      -30,  65,  25},
  /* 98 */ {"rain_detected",      25, -15, -10},   // mellow, slightly subdued
  /* 99 */ {"storm_warning",     -40,  70, -35},   // more alarming
  /*100 */ {"sunny",              65,  35,  25},
  /*101 */ {"cloudy",              5, -30,  -5},   // more subdued
  /*102 */ {"snowing",            35, -15,  -5},   // calm wonder, not excited
  /*103 */ {"windy",               5,  45, -15},   // more energetic
  /*104 */ {"motion_detected",   -15,  80, -25},   // much more alert
  /*105 */ {"phone_ringing",       5,  75, -15},   // startled
  /*106 */ {"timer_done",         45,  55,  25},
  /*107 */ {"calendar_event",     25,  40,  15},
  /*108 */ {"trash_day",         -15,  15,  15},
  /*109 */ {"water_leak",        -80, 100, -60},
  /*110 */ {"co2_high",          -50,  60, -25},   // more alarming
  /*111 */ {"noise_alert",       -40,  80, -30},   // alarm palette
  /*112 */ {"sunrise",            55,  25,  10},
  /*113 */ {"sunset",             50, -35,   5},   // noticeably drowsier
  /*114 */ {"power_outage",      -65,  85, -55},
  /*115 */ {"backup_power",      -25,  50,  10},   // more tense
  /*116 */ {"device_offline",    -35,  40, -15},   // more concerned
  /*117 */ {"new_device",         45,  50,  15},   // more curious
  /*118 */ {"being_petted",       90,  15,   0},   // very content
  /*119 */ {"treat_received",     95,  55,  20},
  /*120 */ {"scolded",           -65,  35, -65},
  /*121 */ {"praised",            85,  45,  35},
  /*122 */ {"stranger_detect",   -35,  85, -55},   // more alarmed
  /*123 */ {"pet_detected",       65,  55,  15},
  /*124 */ {"car_arriving",       40,  60,  15},   // more alert
  /*125 */ {"car_leaving",       -25,  20, -15},
  /*126 */ {"garden_dry",        -25,  25,  -5},
  /*127 */ {"plant_happy",        55,  10,  25},
  /*128 */ {"fridge_open",        15,  50, -15},   // more curious
  /*129 */ {"goodnight",          45, -65,   0},

  // ===== TIME-BASED (130-159) =====
  /*130 */ {"dawn",               40, -35,  -5},
  /*131 */ {"early_morning",      25,  15, -15},
  /*132 */ {"morning_coffee",     55,  30,  25},
  /*133 */ {"midmorning",         50,  40,  35},
  /*134 */ {"noon",               45,  50,  35},   // peak energy
  /*135 */ {"afternoon_slump",   -25, -45, -15},
  /*136 */ {"late_afternoon",     30,   5,  10},
  /*137 */ {"golden_hour",        75, -15,  20},   // warmer, more content
  /*138 */ {"dusk",               40, -35,  -5},
  /*139 */ {"twilight",           25, -50, -15},   // drowsier
  /*140 */ {"early_evening",      50,  10,  20},
  /*141 */ {"dinner_time",        65,  35,  25},   // more distinct from evening
  /*142 */ {"relaxed_evening",    60, -35,  20},
  /*143 */ {"movie_night",        50, -25,  10},
  /*144 */ {"late_night",        -15, -55, -15},
  /*145 */ {"midnight",          -15, -65,   0},
  /*146 */ {"deep_night",          0, -85,   0},
  /*147 */ {"witching_hour",     -20, -30,  20},   // slightly eerie, more dominant
  /*148 */ {"pre_dawn",           10, -55,  -5},
  /*149 */ {"weekend_morning",    65, -15,  25},
  /*150 */ {"sunday_lazy",        50, -55,  10},
  /*151 */ {"monday_blues",      -35, -25, -15},
  /*152 */ {"friday_vibes",       75,  65,  35},
  /*153 */ {"lunch_time",         55,  35,  25},
  /*154 */ {"naptime",            30, -85, -20},
  /*155 */ {"workout_time",       50,  80,  55},   // higher arousal
  /*156 */ {"bedtime",            25, -65, -10},
  /*157 */ {"wee_hours",         -15, -85,   0},
  /*158 */ {"siesta",             35, -75, -10},
  /*159 */ {"tea_time",           65, -25,  25},   // warmer, more content

  // ===== SPECIAL (160-199) =====
  /*160 */ {"christmas",          90,  65,  35},
  /*161 */ {"halloween",          30,  70,  15},   // spookier, less happy
  /*162 */ {"valentines",         95,  35,  10},
  /*163 */ {"new_years",          85,  95,  40},
  /*164 */ {"birthday",           95,  85,  30},
  /*165 */ {"fireworks",          65,  95,  10},
  /*166 */ {"spring_bloom",       65,  35,  10},
  /*167 */ {"summer_heat",       -15,  45,  -5},
  /*168 */ {"autumn_cozy",        65, -25,  25},
  /*169 */ {"winter_cold",       -30,  15, -15},   // colder feeling
  /*170 */ {"aurora",             75,  40, -10},    // wonder, slightly submissive
  /*171 */ {"rainbow",            85,  50,  10},
  /*172 */ {"meditation",         55, -90,  30},
  /*173 */ {"gaming",             65,  75,  45},
  /*174 */ {"reading",            55, -35,  25},
  /*175 */ {"working",            20,  45,  55},
  /*176 */ {"creative_flow",      75,  55,  65},
  /*177 */ {"spa_day",            75, -55,  10},
  /*178 */ {"dance_party",        85,  95,  35},
  /*179 */ {"candlelight",        65, -50,  10},   // calmer, more intimate
  /*180 */ {"stargazing",         55, -25, -10},   // wonder
  /*181 */ {"ocean_vibes",        50, -30,  -5},
  /*182 */ {"forest_bath",        65, -45,  15},
  /*183 */ {"campfire",           65, -15,  25},
  /*184 */ {"thunderstorm",      -35,  85, -30},   // much more intense
  /*185 */ {"snowfall",           40, -25,  -5},
  /*186 */ {"wind_howling",      -15,  50, -15},
  /*187 */ {"heatwave",          -45,  35, -15},
  /*188 */ {"earthquake",        -75, 100, -75},
  /*189 */ {"good_morning",       65,  35,  25},
  /*190 */ {"goodnight_kiss",     85, -35,   0},
  /*191 */ {"tax_season",        -45,  45,  25},
  /*192 */ {"payday",             85,  70,  45},   // more excited
  /*193 */ {"cleaning_day",       25,  35,  25},
  /*194 */ {"baking",             75,  35,  25},
  /*195 */ {"garden_time",        65,  25,  25},
  /*196 */ {"road_trip",          75,  65,  35},   // more adventurous
  /*197 */ {"sick_day",          -65, -45, -45},
  /*198 */ {"celebration",        95,  85,  35},
  /*199 */ {"mystery",            -5,  35, -40},   // suspicious, uncertain
};
