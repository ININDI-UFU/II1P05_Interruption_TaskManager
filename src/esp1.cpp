#include <Arduino.h>
#include <Wire.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/display_ssd1306.h"

constexpr uint8_t def_pin_D1 = 23;
constexpr uint8_t def_pin_D2 = 19;
constexpr uint8_t def_pin_SCL = 22;
constexpr uint8_t def_pin_SDA = 21;

void blinkLEDFunc(uint8_t pin) {
  digitalWrite(pin, !digitalRead(pin));
}

void setup() {
  wserial.begin();
  
  Wire.begin(def_pin_SDA, def_pin_SCL);
  disp.begin(Wire);

  net.begin(KIT_HOSTNAME);

  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, KIT_HOSTNAME);

  pinMode(def_pin_D1, OUTPUT);
  pinMode(def_pin_D2, OUTPUT);
}

void loop() {
  net.update();
  wserial.update();
  disp.update();

  const uint32_t now = millis();

  static uint32_t t1 = 0;
  if (now - t1 >= 1500) {
    t1 = now;
    blinkLEDFunc(def_pin_D1);
  }

  static uint32_t t2 = 0;
  if (now - t2 >= 1000) {
    t2 = now;
    blinkLEDFunc(def_pin_D2);
  }
}
