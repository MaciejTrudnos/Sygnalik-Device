#include "display_config.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "CST816D.h"

#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1

CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

bool deviceConnected = false;
String message = "Brak powiadomien";

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

lv_obj_t *label;

class MyCallbacks : public BLECharacteristicCallbacks 
{
  void onWrite(BLECharacteristic *pCharacteristic) 
  {
    String rxValue = pCharacteristic->getValue().c_str();

    if (rxValue.length() > 0) 
    {
      Serial.println(rxValue);
      message = rxValue;
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks 
{
  void onConnect(BLEServer *pServer) 
  {
    deviceConnected = true;
    Serial.println("Urządzenie połączone!");
  };

  void onDisconnect(BLEServer *pServer) 
  {
    deviceConnected = false;
    Serial.println("Urządzenie rozłączone!");

    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Start...");
  touch.begin();
  display_init();
  label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);

  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(0, INPUT);

  BLEDevice::init("Sygnalik");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Sygnalik");

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();

  Serial.println("Setup done");
}

void loop() {
  lv_timer_handler();
  delay(5);

  if (deviceConnected) {
    setLabel(message.c_str());
    checkTouch();
  } else {
    setLabel("Polacz z urzadzeniem");
  }
}

void setLabel(const char *txt) {
  lv_label_set_text(label, txt);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void checkTouch() {
  uint16_t touchX, touchY;
  uint8_t gesture;
  
  bool touched = touch.getTouch(&touchX, &touchY, &gesture);
  
  if (touched) {
    Serial.printf("Ekran dotknięty! X:%d, Y:%d, gesture: %x\n", touchX, touchY, gesture);
    message = "Brak powiadomien";
  }
}
