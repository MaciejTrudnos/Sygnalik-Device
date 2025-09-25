#include "display_config.h"

lv_obj_t *label;
bool new_notification = true;

void setup()
{
  Serial.begin(115200);
  Serial.println("Start...");

  display_init();
  label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);
  setLabel("Uruchamianie...");

  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(0, INPUT);

  Serial.println("Setup done");
}

void loop()
{
  lv_timer_handler(); // let LVGL work
  delay(5);

  if (new_notification) {
    setLabel("Brak powiadomien");
    new_notification = false;
  }
}

void setLabel(const char *txt) {
  lv_label_set_text(label, txt);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}