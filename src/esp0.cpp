#include "Arduino.h"
#include <EEPROM.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/ads1115.h"
#include "services/display_ssd1306.h"
#include "util/lasecDebounce.h"

constexpr uint8_t def_pin_D1 = 23;
constexpr uint8_t def_pin_D2 = 19;
constexpr uint8_t def_pin_SCL = 22;     ///< GPIO para SCL do display OLED.
constexpr uint8_t def_pin_SDA = 21;     ///< GPIO para SDA do display OLED.

lasecDebounce push1,push2,rtn1,rtn2;
bool adsOk = false;

void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

void managerInputFunc(void) {
    if (!adsOk) return;

    const int16_t vlPOT1 = ads1115.analogReadPot1();
    const int16_t vlPOT2 = ads1115.analogReadPot2();
    wserial.plot("vlPOT1", vlPOT1);
    wserial.plot("vlPOT2", vlPOT2);

    static uint32_t previousDisplayMS = 0;
    const uint32_t currentTimeMS = millis();
    if ((currentTimeMS - previousDisplayMS) >= 250)
    {
      previousDisplayMS = currentTimeMS;
      disp.setText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());
    }
}

void setup()
{
  wserial.begin();
  disp.begin(def_pin_SDA, def_pin_SCL);
  adsOk = ads1115.begin();
  net.begin(KIT_HOSTNAME);
  
  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  if (adsOk) {
    disp.setText(2, ("ADS OK 0x" + String(ads1115.address(), HEX)).c_str());
    wserial.println("ADS OK 0x" + String(ads1115.address(), HEX));
  } else {
    disp.setText(2, "ADS ERRO I2C");
    wserial.println("ADS ERRO I2C");
  }
  disp.setText(3, "");

  pinMode(def_pin_D1, OUTPUT);
  pinMode(def_pin_D2, OUTPUT);  
  
  push1.begin(18, INPUT_PULLUP, LOW, 20);
  
  delay(50);  
}

void loop()
{
  wserial.update();
  disp.update();
  net.update();
  
  const uint64_t currentTimeMS = millis();

  static uint64_t previousTimeMS1 = 0;
  if ((currentTimeMS - previousTimeMS1) >= 1500)
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
