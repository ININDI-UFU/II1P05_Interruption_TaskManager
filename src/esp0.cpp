#include <Arduino.h>
#include <Wire.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/display_ssd1306.h"

constexpr uint8_t def_pin_SCL = 22;
constexpr uint8_t def_pin_SDA = 21;

void setup() {
  wserial.begin();

  Wire.begin(def_pin_SDA, def_pin_SCL);
  disp.begin(Wire);

  net.begin(KIT_HOSTNAME);

  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, KIT_HOSTNAME);
}

void loop() {
  net.update();
  wserial.update();
  disp.update();
}
