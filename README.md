# Sygnalik-Device
A dedicated device that receives smartphone notifications via Bluetooth and displays them safely for motorcycle riders.

## Motivation
The device was designed to replace a smartphone in a motorcycle mount with a separate dedicated device.  
This allows the phone to stay safely stored, while the device connects via Bluetooth, reducing the risk of damage while riding.

## About the Device
The **Sygnalik-Device** connects via Bluetooth to the [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App) and displays incoming messages directly on the device.

## Supported Notification Types
- 📩 **SMS** – shows sender and message preview  
- 📞 **Incoming calls** – shows caller ID / contact name  
- 🚨 **Speed camera alerts** – shows warning icon and distance

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
