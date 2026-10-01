# AGENTS.md

Guidance for AI coding agents working on **Sygnalik-Device**. Read this before making changes.

## Project overview

Sygnalik-Device is the in-vehicle hardware companion for the [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App) Android app.

- **Hardware:** ESP32-2424S012 (ESP32-C3 based) with a 1.28" round 240x240 display.
- **Role:** connects to the phone over **Bluetooth** and shows notifications in real time. The phone is the source of truth; the device is a display and alert unit.
- **Screens/alerts:** connect screen, main screen, incoming call, SMS, speed camera warning, speed control warning.

The device runs unattended in a vehicle, so it must be **reliable, quick to boot, and never freeze** on a bad or partial message.

## Tech stack

- **Language:** C++ (Arduino framework for ESP32)
- **UI:** LVGL (v8-style API, e.g. `lv_img_dsc_t`, `LV_IMG_CF_TRUE_COLOR`)
- **Toolchain:** Arduino IDE
- **Sketch folder:** `Sygnalik/`
- **Assets:** `assets/` (README photos and previews only, not firmware assets)

> Do not upgrade LVGL or the ESP32 Arduino core on your own. LVGL 8 to 9 changes the API significantly, and display/touch driver config is tied to the library version.

## Repository layout

```
Sygnalik-Device/
├── Sygnalik/              # Arduino sketch: firmware source
│   ├── Sygnalik.ino       # Entry point: BLE handling, message parsing, screen switching
│   ├── display_config.*   # GC9A01 round display setup (LovyanGFX) + LVGL init/flush
│   ├── CST816D.*          # Touch controller driver (I2C)
│   ├── nav.h / nav.cpp    # Navigation screen (maneuver icon + distance glyphs)
│   ├── nav_icons.h/.cpp   # Auto-generated maneuver icons + distance glyphs (do not edit)
│   ├── connectdevice.cpp  # Generated full-screen image: connect screen
│   ├── phone.cpp          # Generated full-screen image: incoming call alert
│   ├── sms.cpp            # Generated full-screen image: SMS alert
│   ├── speed.cpp          # Generated full-screen image: speed camera warning
│   ├── speedcontrolwarning.cpp # Generated full-screen image: speed control warning
│   └── suzuki.cpp         # Generated full-screen image: main screen
├── assets/                # Images used by README (incl. nav-icons/ PNG sources)
└── README.md
```

Generated `*_map[]` pixel arrays come from the LVGL Image Converter (full screens)
or the nav-icons conversion script (nav_icons.cpp). Never hand-edit them.

## Build and flash

Build and upload are done in **Arduino IDE** (no CLI build configured in the repo). Required board settings:

| Option | Value |
| --- | --- |
| Board | ESP32C3 Dev Module |
| Upload Speed | 921600 |
| USB CDC On Boot | Enabled |
| CPU Frequency | 160MHz (WiFi) |
| Flash Frequency | 80MHz |
| Flash Mode | QIO |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Huge APP (3MB No OTA / 1MB SPIFFS) |
| JTAG Adapter | Integrated USB JTAG |

Rules for agents:

- **You cannot flash or run the firmware.** Never claim a change "works" on the device. State what compiles in theory and what must be tested on real hardware.
- If you can compile (e.g. `arduino-cli` is installed locally), use the FQBN and options matching the table above. Do not add CLI tooling or new build files to the repo without asking.
- Do not change the board settings or partition scheme. "Huge APP" has **no OTA**, so firmware size matters: watch flash usage when adding fonts, images, or libraries.

## Embedded constraints

The ESP32-C3 is small: single core, limited RAM, 4MB flash.

- **No dynamic allocation in hot paths** (loops, callbacks, redraws). Prefer static or pre-allocated buffers. Avoid `String` concatenation in loops; prefer fixed `char[]` buffers with bounds-checked functions (`snprintf`, `strlcpy`).
- **Never block** in `loop()` or in Bluetooth/LVGL callbacks. No long `delay()`. LVGL's `lv_timer_handler()` must run regularly.
- **Check buffer sizes** on every incoming Bluetooth message. Treat all data from the phone as untrusted: validate length, handle truncated or malformed payloads, never assume null termination.
- **LVGL is not thread-safe.** Do not touch LVGL objects from a Bluetooth callback or another task unless a lock/queue pattern is already used in the code. Prefer setting a flag or posting to a queue and updating UI from the main loop.
- Keep **stack usage** low; no large local arrays.
- Minimize flash: large images and fonts eat the 3MB app partition quickly.

## Bluetooth protocol

The message format between **Sygnalik-App** (sender) and this device (receiver) is a **contract shared by two repos**.

- Plain-text messages written to the characteristic; known types: `sms`, `call`, `speedcamera`, `speedcontrol`.
- Navigation: `nav|<maneuver>|<distance text>` where `<maneuver>` is an icon key from `assets/nav-icons/` minus the `direction_` prefix (e.g. `turn_right`, `roundabout_left`, `depart`, `arrive`, `uturn`) and `<distance text>` is app-formatted display text (e.g. `450 m`, `1.2 km`, max 15 chars, ASCII, no `|`). `navoff` ends navigation. Unknown maneuver keys fall back to a generic icon; unknown message types are ignored.
- Do not change message formats, field order, delimiters, or UUIDs without flagging it clearly. The Android app must be changed in lockstep.
- When adding a new alert type, document the message format in your summary so it can be implemented on the app side.
- Handle disconnects and reconnects cleanly: on disconnect, return to the connect screen and resume advertising; never leave stale alerts displayed forever.
- Ignore unknown message types instead of crashing, so a newer app does not break an older firmware.

## UI and LVGL conventions

- Display is **round 240x240**: keep important content inside the visible circle; corners are cut off.
- Alerts must be **readable at a glance** while driving: large text, high contrast, minimal words.
- Alerts need a defined lifetime (auto-dismiss or replaced by the next state). Don't leave a call/SMS/camera alert stuck on screen.
- Keep one screen/alert per function and reuse existing styles instead of duplicating style code.
- Do not hardcode pixel positions in many places; reuse constants.

### Adding images (LVGL Image Converter)

Use the [LVGL Image Converter](https://lvgl.io/tools/imageconverter) with:

1. Color Format: `CF_TRUE_COLOR`
2. Output Format: `C array`
3. Save the output with a `*.cpp` extension
4. Make the image descriptor a `const lv_img_dsc_t`, following the existing pattern for width/height/`data_size` (240x240 full-screen images use `57600 * LV_COLOR_SIZE / 8`)

Keep images `const` so they live in flash, not RAM. Prefer small icons over full-screen images where possible to save flash. Do not hand-edit generated pixel arrays.

## Code conventions

- Follow the style of the surrounding code; do not reformat whole files.
- Prefer small, named functions over long `loop()` or callback bodies.
- Use `constexpr`/`const` instead of `#define` for new constants where practical.
- Avoid global mutable state when a small struct or function parameter will do. Where globals are already the pattern, keep changes consistent.
- No floating-point-heavy or `std::` container-heavy code unless needed; mind flash size.
- Add short comments for hardware-specific details (pins, timings, display quirks), not for obvious code.
- Serial debug output is fine during development but keep it concise and never print message contents in a way that spams the log in the main loop.

## Hardware and safety

- Do not change **pin assignments** or display/touch driver settings unless the task is explicitly about hardware setup. Wrong pin changes can render the display unusable until reflashed.
- Do not add Wi-Fi, OTA, or web-server features without asking: the partition scheme has no OTA slot, and extra radios affect power and stability in a vehicle.
- The unit is powered from a car. Avoid features that increase power draw noticeably (e.g. full-brightness always-on without need) unless requested.

## Testing expectations

There is no automated test setup. Therefore:

- Keep parsing and message-handling logic in small functions that could be tested on a PC later.
- In your summary, list a **manual test checklist** for the change, for example: connect screen appears on boot, connection to the app succeeds, each alert type displays and clears, disconnect returns to connect screen.
- Clearly separate "compiles / reviewed" from "verified on hardware".

## Git and PR workflow

- Default branch: `main`. Use a feature branch per task.
- Commit messages: short imperative subject (e.g. `Add speed control warning screen`).
- Do not push to `main` directly or rewrite history.
- Do not commit build output, local IDE settings, or large binary files other than intentional image data.

## Boundaries

**Always:** validate incoming Bluetooth data; keep the main loop non-blocking; keep flash and RAM usage in mind; list manual hardware tests.

**Ask first:** changing the Bluetooth message format or UUIDs; adding libraries; upgrading LVGL or the ESP32 core; changing board options or partition scheme; adding Wi-Fi/OTA; changing pin or display driver config.

**Never:** claim hardware behavior was verified when it wasn't; block the UI thread; call LVGL from interrupt/Bluetooth callbacks unsafely; commit credentials; hand-edit generated image arrays.

## Related repositories

- [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App): Android app that sends the notifications and alerts
- [Sygnalik-Directions-API](https://github.com/MaciejTrudnos/Sygnalik-Directions-API): turn-by-turn directions backend
