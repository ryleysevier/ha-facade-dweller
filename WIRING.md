# Wiring: XIAO ESP32-C6 → Elecrow 1.28" Round LDC (GC9A01)

```
  Elecrow GC9A01              XIAO ESP32-C6
  ┌──────────┐                ┌──────────────┐
  │          │                │              │
  │  VCC  ●──┼────────────────┼── 3.3V       │
  │  GND  ●──┼────────────────┼── GND        │
  │  SDA  ●──┼────────────────┼── D10 (GP18) │  ← MOSI
  │  SCL  ●──┼────────────────┼── D8  (GP19) │  ← SCK
  │  CS   ●──┼────────────────┼── D1  (GP8)  │  ← Chip Select
  │  DC   ●──┼────────────────┼── D0  (GP9)  │  ← Data/Command
  │  RST  ●──┼────────────────┼── D2  (GP2)  │  ← Reset
  │  BL   ●──┼────────────────┼── D3  (GP3)  │  ← Backlight
  │          │                │              │
  └──────────┘                └──────────────┘
```

## Pin Summary

| LDC Pin | Function      | XIAO Pin | GPIO |
|---------|---------------|----------|------|
| VCC     | Power (3.3V)  | 3V3      | —    |
| GND     | Ground        | GND      | —    |
| SDA     | SPI MOSI      | D10      | 18   |
| SCL     | SPI Clock     | D8       | 19   |
| CS      | Chip Select   | D1       | 8    |
| DC      | Data/Command  | D0       | 9    |
| RST     | Reset         | D2       | 2    |
| BL      | Backlight     | D3       | 3    |

## Notes

- All signals are 3.3V — no level shifting needed
- SCL/SDA are SPI labels (not I2C) — this is an SPI display
- DC is the same as DC (Data/Command) on other modules
- BL is driven HIGH in firmware for full brightness (PWM possible on D3 for dimming)
