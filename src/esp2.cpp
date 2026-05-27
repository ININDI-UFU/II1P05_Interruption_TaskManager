#include <Arduino.h>
#include <Wire.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/display_ssd1306.h"
#include "util/lasecDebounce.h"

constexpr uint8_t def_pin_PUSH2 = 32; // tem pull-down interno
constexpr uint8_t def_pin_D4 = 4;
constexpr uint8_t def_pin_SCL = 22;
constexpr uint8_t def_pin_SDA = 21;

lasecDebounce btn;

void setup() {
  wserial.begin();
  
  Wire.begin(def_pin_SDA, def_pin_SCL);
  disp.begin(Wire);

  net.begin(KIT_HOSTNAME);

  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, "BTN GPIO32");

  pinMode(def_pin_D4, OUTPUT); digitalWrite(def_pin_D4, LOW);  
  btn.begin(def_pin_PUSH2, INPUT_PULLDOWN, HIGH, 20);
}

void loop() {
  net.update();
  wserial.update();
  disp.update();

  btn.tick([](lasecDebounce::Event event) {
    if (event == lasecDebounce::Pressed) {
      digitalWrite(def_pin_D4, HIGH);
      wserial.println("button pressed");
    }

    if (event == lasecDebounce::Released) {
      digitalWrite(def_pin_D4, LOW);
      wserial.println("button released");
    }
  });
}
