#include <Arduino.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/display_ssd1306.h"
#include "util/lasecDebounce.h"

constexpr uint8_t def_pin_PUSH1 = 34;
constexpr uint8_t def_pin_D3 = 18;
constexpr uint8_t def_pin_SCL = 22;
constexpr uint8_t def_pin_SDA = 21;

lasecDebounce push1;

void setup() {
  wserial.begin();
  disp.begin(def_pin_SDA, def_pin_SCL);
  net.begin(KIT_HOSTNAME);

  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, KIT_HOSTNAME);

  pinMode(def_pin_D3, OUTPUT);
  push1.begin(def_pin_PUSH1, INPUT, HIGH, 20);
}

void loop() {
  net.update();
  wserial.update();
  disp.update();

  push1.tick([](lasecDebounce::Event event) {
    if (event == lasecDebounce::Pressed) {
      digitalWrite(def_pin_D3, HIGH);
      wserial.println("push1 pressed");
    }

    if (event == lasecDebounce::Released) {
      digitalWrite(def_pin_D3, LOW);
      wserial.println("push1 released");
    }
  });
}
