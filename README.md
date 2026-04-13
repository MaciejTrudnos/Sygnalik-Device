# Sygnalik-Device  

A dedicated device designed for the [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App)

## Overview  
The device ESP32-2424S012 with a 1.28" display connects to the [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App) via Bluetooth and receives notifications in real time.

## Features  
- [x] **SMS alerts** – Get notified of incoming texts on your device.
- [x] **Call alerts** – Real-time incoming call notifications.
- [x] **Speed camera alerts** – Stay safe with localized alerts (Poland).
- [x] ~~**Traccar integration** – Automatic GPS route tracking.~~ (Sygnalik App)
- [ ] **Navigation support** – (In progress) Visual [turn-by-turn](https://github.com/MaciejTrudnos/Sygnalik-Directions-API) cues.

## Dev Module Configuration in Arduino IDE

| Option                           | Value                                      |
|----------------------------------|--------------------------------------------|
| **Board**                        | ESP32C3 Dev Module                         |
| **Upload Speed**                 | 921600                                     |
| **USB CDC On Boot**              | Enabled                                    |
| **CPU Frequency**                | 160MHz (WiFi)                              |
| **Flash Frequency**              | 80MHz                                      |
| **Flash Mode**                   | QIO                                        |
| **Flash Size**                   | 4MB (32Mb)                                 |
| **Partition Scheme**             | Huge APP (3MB No OTA / 1MB SPIFFS)        |
| **JTAG Adapter**                 | Integrated USB JTAG                        |

## Image Converter for LVGL
1. Go to [LVGL Image Converter](https://lvgl.io/tools/imageconverter)  
2. Select an image (BMP, JPG, PNG, or SVG)  
3. Set **Color Format**: `CF_TRUE_COLOR`  
4. Set **Output Format**: `C array`  
5. Convert and save the file with a `*.cpp` extension  
6. Update your file with the const variable format:

```cpp
const lv_img_dsc_t variablename = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .always_zero = 0,
        .reserved = 0,
        .w = 240,
        .h = 240,
    },
    .data_size = 57600 * LV_COLOR_SIZE / 8,
    .data = variablename_map,
};
```

## Preview alerts

| Preview | Description |
|--------|-------------|
| <img src="https://raw.githubusercontent.com/MaciejTrudnos/Sygnalik-Device/main/assets/connectdevice.jpg" width="120"> | Connect to the Sygnalik app |
| <img src="https://raw.githubusercontent.com/MaciejTrudnos/Sygnalik-Device/main/assets/suzuki.jpg" width="120"> | Main device screen. |
| <img src="https://raw.githubusercontent.com/MaciejTrudnos/Sygnalik-Device/main/assets/phone.jpg" width="120"> | Incoming call alert |
| <img src="https://raw.githubusercontent.com/MaciejTrudnos/Sygnalik-Device/main/assets/sms.jpg" width="120"> | SMS alert |
| <img src="https://raw.githubusercontent.com/MaciejTrudnos/Sygnalik-Device/main/assets/speed.jpg" width="120"> | Speed camera warning |

## Real Device Photos

<table align="left">
  <tr>
    <td align="center">
      <img src="https://github.com/MaciejTrudnos/Sygnalik-Device/blob/main/assets/mysuzuki.jpg" height="260"><br>
      <sub><b>Installed in vehicle – front view</b></sub>
    </td>
    <td align="center">
      <img src="https://github.com/MaciejTrudnos/Sygnalik-Device/blob/main/assets/mysuzuki-2.jpg" height="260"><br>
      <sub><b>Installed in vehicle – side view</b></sub>
    </td>
  </tr>
</table>
