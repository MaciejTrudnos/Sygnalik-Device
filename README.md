# Sygnalik-Device  

The Sygnalik-Device receives smartphone messages via Bluetooth from the [Sygnalik-App](https://github.com/MaciejTrudnos/Sygnalik-App) and displays them on the device.  

## Supported notification types  
- 📩 SMS
- 📞 Incoming calls
- 🚨 Speed camera alerts

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
| **Partition Scheme**             | Huge APP (3MB No OTA / 1MB SPIFFS)         |
| **JTAG Adapter**                 | Integrated USB JTAG                        |

## Image Converter to use in LVGL

1. Go to [Image Converter](https://lvgl.io/tools/imageconverter)
2. Select image (BMP, JPG, PNG, or SVG)
3. Set color format: `CF_TRUE_COLOR`
4. Set output format: `C array`
5. Convert and save file with `*.cpp` extension
6. In your file update the const variable using the following format:

```
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