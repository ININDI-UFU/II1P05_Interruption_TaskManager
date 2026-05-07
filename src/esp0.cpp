#include "Arduino.h"
#include "services/wserial.h"
#include "services/ads1115.h"
#include "services/display_ssd1306.h"

constexpr uint8_t def_pin_D1 = 23;
constexpr uint8_t def_pin_D2 = 19;
constexpr uint8_t def_pin_SCL = 22;     ///< GPIO para SCL do display OLED.
constexpr uint8_t def_pin_SDA = 21;     ///< GPIO para SDA do display OLED.

void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

void managerInputFunc(void) {
    const uint16_t vlPOT1 = ads1115.analogReadPot1();
    const uint16_t vlPOT2 = ads1115.analogReadPot2();
    disp.setText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());
    wserial.plot("vlPOT1", vlPOT1);
    wserial.plot("vlPOT2", vlPOT2);
}

void setup()
{
  wserial.begin();
  if (disp.begin(def_pin_SDA, def_pin_SCL)) {
      disp.setText(1, "Inicializando...");
      disp.setText(2, "WIFI not connected");
      disp.setText(3, "Display Ativo");
  } else {
      wserial.println("Falha ao iniciar o display OLED");
  }
  ads1115.begin();
  pinMode(def_pin_D1, OUTPUT);
  pinMode(def_pin_D2, OUTPUT);  
  delay(50);  
}

void loop()
{
  wserial.update();
  disp.update();
  const uint64_t currentTimeMS = millis();

  static uint64_t previousTimeMS1 = 0;
  if ((currentTimeMS - previousTimeMS1) >= 500)
  {
    previousTimeMS1 = currentTimeMS;
    blinkLEDFunc(def_pin_D1);
  }

  static uint64_t previousTimeMS2 = 0;
  if ((currentTimeMS - previousTimeMS2) >= 1000)
  {
    previousTimeMS2 = currentTimeMS;
    blinkLEDFunc(def_pin_D2);
  }

  static uint64_t previousTimeMS3 = 0;  
  if ((currentTimeMS - previousTimeMS3) >= 50)
  {
    previousTimeMS3 = currentTimeMS;
    managerInputFunc();
  } 
}