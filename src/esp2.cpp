//https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
#include <Arduino.h>   // Biblioteca base do framework Arduino, necessária para funções básicas como Serial e delays.

#define NUMTASKS 3
#include "util/jtask.h"
#include "services/wserial.h"
#include "services/ads1115.h"
#include "services/display_ssd1306.h"

constexpr uint8_t def_pin_D1 = 23;
constexpr uint8_t def_pin_D2 = 19;
constexpr uint8_t def_pin_SCL = 22;     ///< GPIO para SCL do display OLED.
constexpr uint8_t def_pin_SDA = 21;     ///< GPIO para SDA do display OLED.

//Funçao de alterar o estado de um led
void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

//Função que le os valores dos POT e das Entradas 4 a 20 mA e plota no display
void managerInputFunc(void) {
    const uint16_t vlPOT1 = ads1115.analogReadPot1();
    const uint16_t vlPOT2 = ads1115.analogReadPot2();
    disp.setText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());    
    wserial::plot("vlPOT1", vlPOT1);
    wserial::plot("vlPOT2", vlPOT2);
}

//Configuração inicial do programa
void setup() {
    disp.start(def_pin_SDA, def_pin_SCL);    
    jtaskSetup(1000);
    jtaskAttachFunc(managerInputFunc, 50); //anexa um função e sua base de tempo para ser executada
    jtaskAttachFunc([](){blinkLEDFunc(def_pin_D1);}, 500);  //anexa um função e sua base de tempo para ser executada
    jtaskAttachFunc([](){blinkLEDFunc(def_pin_D2);}, 1000);  //anexa um função e sua base de tempo para ser executada
}

//Loop principal
void loop() {
  disp.update();    
  jtaskLoop();
}