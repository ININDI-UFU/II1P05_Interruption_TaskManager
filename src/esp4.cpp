#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>
#include "services/lasecNet.h"
#include "services/wserial.h"
#include "services/ads1115.h"
#include "services/display_ssd1306.h"
#include "util/lasecDebounce.h"

constexpr uint8_t def_pin_D1 = 23;
constexpr uint8_t def_pin_D2 = 19;
constexpr uint8_t def_pin_D3 = 18;  
constexpr uint8_t def_pin_D4 = 4;

constexpr uint8_t def_pin_RTN1    =  2;
constexpr uint8_t def_pin_RTN2    = 35;  ///< GPIO para botão retentivo 2. INPUT-ONLY no ESP32 — sem pull interno.
constexpr uint8_t def_pin_PUSH1   = 34;  ///< GPIO para botão push 1. INPUT-ONLY no ESP32 — sem pull interno.  
constexpr uint8_t def_pin_PUSH2   = 32; 

constexpr uint8_t def_pin_SCL = 22;     ///< GPIO para SCL do display OLED.
constexpr uint8_t def_pin_SDA = 21;     ///< GPIO para SDA do display OLED.

lasecDebounce push1,push2,rtn1,rtn2;
bool adsOk = false;

void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

void managerDebounceFunc(const char *name, lasecDebounce::Event event) {
    switch (event) {
        case lasecDebounce::Pressed:
            wserial.println(String(name) + " pressed");
            break;

        case lasecDebounce::Released:
            wserial.println(String(name) + " released");
            break;

        case lasecDebounce::Held:
            break;

        case lasecDebounce::None:
        default:
            break;
    }
}

void managerInputFunc(void) {
    if (!adsOk) return;
    const int16_t vlPOT1 = ads1115.analogReadPot1();
    const int16_t vlPOT2 = ads1115.analogReadPot2();
    wserial.plot("vlPOT1", vlPOT1);
    wserial.plot("vlPOT2", vlPOT2);
    disp.setText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());
}

void setup()
{
  wserial.begin();
  Wire.begin(def_pin_SDA, def_pin_SCL);
  disp.begin(Wire);
  adsOk = ads1115.begin(Wire);
  net.begin(KIT_HOSTNAME);
  
  disp.setText(1, (WiFi.localIP().toString() + " ID:" + String(KIT_ID)).c_str());
  disp.setText(2, "");
  if (adsOk) {
    disp.setText(3, ("ADS OK 0x" + String(ads1115.address(), HEX)).c_str());
    wserial.println("ADS OK 0x" + String(ads1115.address(), HEX));
  } else {
    disp.setText(3, "ADS ERRO I2C");
    wserial.println("ADS ERRO I2C");
  }

  pinMode(def_pin_D1, OUTPUT);
  pinMode(def_pin_D2, OUTPUT);  
  pinMode(def_pin_D3, OUTPUT);
  pinMode(def_pin_D4, OUTPUT);
  
  digitalWrite(def_pin_D3, LOW);
  digitalWrite(def_pin_D4, LOW);

  push1.begin(def_pin_PUSH1, INPUT, HIGH, 20);
  push2.begin(def_pin_PUSH2, INPUT_PULLDOWN, HIGH, 20);
  rtn1.begin(def_pin_RTN1, INPUT_PULLDOWN, HIGH, 20);
  rtn2.begin(def_pin_RTN2, INPUT, HIGH, 20);

  delay(50);  
}

void loop()
{
  net.update();
  wserial.update();
  disp.update();

  push1.tick([](lasecDebounce::Event event) {
    if (event == lasecDebounce::Pressed) {
      digitalWrite(def_pin_D3, HIGH);
      wserial.println("push1 pressed -> D3 HIGH");
    } else if (event == lasecDebounce::Released) {
      digitalWrite(def_pin_D3, LOW);
      wserial.println("push1 released -> D3 LOW");
    }
  });

  push2.tick([](lasecDebounce::Event event) {
    if (event == lasecDebounce::Held) {
      static uint32_t previousBlinkMS = 0;
      const uint32_t currentTimeMS = millis();
      if ((currentTimeMS - previousBlinkMS) >= 250) {
        previousBlinkMS = currentTimeMS;
        blinkLEDFunc(def_pin_D4);
      }
    } else if (event == lasecDebounce::Released) {
      digitalWrite(def_pin_D4, LOW);
    }
  });
  rtn1.tick([](lasecDebounce::Event event) { managerDebounceFunc("rtn1", event); });
  rtn2.tick([](lasecDebounce::Event event) { managerDebounceFunc("rtn2", event); });

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
  if ((currentTimeMS - previousTimeMS3) >= 500)
  {
    previousTimeMS3 = currentTimeMS;
    managerInputFunc();
  } 
}
