#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#define LGFX_USE_V1
#include "Arduino.h"
#include <lvgl.h>
#include <LovyanGFX.hpp>

#define off_pin 35
#define buf_size 30

// Define the custom LGFX class for your specific display setup
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

  public:
    LGFX(void)
    {
      { // SPI bus configuration
        auto cfg = _bus_instance.config();

        // SPI bus settings
        cfg.spi_host = SPI2_HOST;         // Use SPI2_HOST for ESP32-C3
        cfg.spi_mode = 0;                 // Set SPI mode (0 ~ 3)
        cfg.freq_write = 80000000;        // SPI clock for write operations (up to 80 MHz)
        cfg.freq_read = 20000000;         // SPI clock for read operations
        cfg.spi_3wire = true;             // Set to true if receiving data on MOSI
        cfg.use_lock = true;              // Set to true to use transaction locks
        cfg.dma_channel = SPI_DMA_CH_AUTO; // Use automatic DMA channel
        cfg.pin_sclk = 6;  // SCLK pin number
        cfg.pin_mosi = 7;  // MOSI pin number
        cfg.pin_miso = -1; // MISO pin number (not used, hence -1)
        cfg.pin_dc = 2;    // D/C pin number

        _bus_instance.config(cfg);              // Apply the SPI bus configuration
        _panel_instance.setBus(&_bus_instance); // Set the bus to the panel
      }

      { // Panel configuration
        auto cfg = _panel_instance.config();

        cfg.pin_cs = 10;   // CS pin number
        cfg.pin_rst = -1;  // RST pin number (not used, hence -1)
        cfg.pin_busy = -1; // BUSY pin number (not used, hence -1)

        // Panel size and offsets
        cfg.memory_width = 240;   // Maximum width supported by the driver IC
        cfg.memory_height = 240;  // Maximum height supported by the driver IC
        cfg.panel_width = 240;    // Actual display width
        cfg.panel_height = 240;   // Actual display height
        cfg.offset_x = 0;         // X-axis offset
        cfg.offset_y = 0;         // Y-axis offset
        cfg.offset_rotation = 0;  // Offset for rotation
        cfg.dummy_read_pixel = 8; // Dummy bits before reading pixels
        cfg.dummy_read_bits = 1;  // Dummy bits before reading non-pixel data
        cfg.readable = false;     // Set to true if the panel can read data
        cfg.invert = true;        // Set to true if the display colors are inverted
        cfg.rgb_order = false;    // Set to true if red and blue are swapped
        cfg.dlen_16bit = false;   // Set to true if data length is sent in 16-bit units
        cfg.bus_shared = false;   // Set to true if the bus is shared with an SD card

        _panel_instance.config(cfg); // Apply the panel configuration
      }

      setPanel(&_panel_instance); // Set the panel for use
    }
};

// Constants
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 240;

// Global variables declarations
extern LGFX tft;
extern lv_disp_draw_buf_t draw_buf;
extern lv_color_t buf[2][screenWidth * buf_size];

// Function declarations
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void tcr1s();
void display_init();

#endif