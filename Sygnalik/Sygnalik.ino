#include "display_config.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "CST816D.h"
#include "sms.cpp"
#include "phone.cpp"
#include "suzuki.cpp"
#include "connectdevice.cpp"
#include "speed.cpp"

#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1

CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

String message = "nonotifications";

int img_angle = -900;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
bool update_img = true;
lv_obj_t *img_obj;

enum message_type {
  donothing,
  noconnection,
  nonotifications,
  textmessage,
  call,
  speedcamera
};

message_type message_state = noconnection;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue().c_str();

    if (rxValue.length() > 0) {
      Serial.println(rxValue);
      update_img = true;
      if (rxValue == "sms") {
        message_state = textmessage;
      } else if (rxValue == "call") {
        message_state = call;
      } else if (rxValue == "speedcamera") {
        message_state = speedcamera;
      }
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    update_img = true;
    message_state = nonotifications;
    Serial.println("Urządzenie połączone!");
  };

  void onDisconnect(BLEServer *pServer) {
    update_img = true;
    message_state = noconnection;
    Serial.println("Urządzenie rozłączone!");

    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Start...");
  touch.begin();
  display_init();

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

  img_obj = lv_img_create(lv_scr_act());
  lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);

  lv_img_set_src(img_obj, &speed);
  lv_img_set_angle(img_obj, img_angle);

  Serial.println("Setup done");
}

void loop() {
  lv_timer_handler();
  delay(5);

  if (message_state == noconnection && update_img) {
    Serial.println("show connectdevice");
    lv_img_set_src(img_obj, &connectdevice);
    update_img = false;
  } else {
    if (message_state == textmessage && update_img) {
      Serial.println("show sms");
      lv_img_set_src(img_obj, &sms);
    } else if (message_state == call && update_img) {
      Serial.println("show call");
      lv_img_set_src(img_obj, &phone);
    } else if (message_state == nonotifications && update_img) {
      Serial.println("no notifications");
      lv_img_set_src(img_obj, &suzuki);
    } else if (message_state == speedcamera && update_img) {
      Serial.println("speedcamera");
      lv_img_set_src(img_obj, &speed);
    }

    update_img = false;
    checkTouch();
  }
}

void checkTouch() {
  uint16_t touchX, touchY;
  uint8_t gesture;

  bool touched = touch.getTouch(&touchX, &touchY, &gesture);

  if (touched) {
    Serial.printf("Ekran dotknięty! X:%d, Y:%d, gesture: %x\n", touchX, touchY, gesture);

    if (message_state != noconnection) {
      message_state = nonotifications;
      update_img = true;
    }
  }
}