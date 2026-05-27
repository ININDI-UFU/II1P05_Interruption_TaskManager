#include <Arduino.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/display_ssd1306.h"
#include "services/ads1115.h"

constexpr uint8_t def_pin_SCL = 22;
constexpr uint8_t def_pin_SDA = 21;

bool adsOk = false;

void managerInputFunc() {
  if (!adsOk) return;

  const int16_t pot1 = ads1115.analogReadPot1();
  const int16_t pot2 = ads1115.analogReadPot2();

  wserial.plot("pot1", pot1);
  wserial.plot("pot2", pot2);
  disp.setText(2, ("P1:" + String(pot1) + " P2:" + String(pot2)).c_str());
}

void setup() {
  wserial.begin();
  disp.begin(def_pin_SDA, def_pin_SCL);
  adsOk = ads1115.begin();
  net.begin(KIT_HOSTNAME);

  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, "");
  disp.setText(3, adsOk ? "ADS OK" : "ADS ERRO");
}

void loop() {
  net.update();
  wserial.update();
  disp.update();

  static uint32_t t = 0;
  if (millis() - t >= 500) {
    t = millis();
    managerInputFunc();
  }
}
