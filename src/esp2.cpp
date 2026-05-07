//https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
#include "IIKit.h"   // Biblioteca base do framework Arduino, necessária para funções básicas como Serial e delays.
#define NUMTASKS 3
#include "util/jtask.h"
#include "util/dinDebounce.h"

//Funçao de alterar o estado de um led
void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

//Função que le os valores dos POT e das Entradas 4 a 20 mA e plota no display
void managerInputFunc(void) {
    const uint16_t vlPOT1 = IIKit.analogReadPot1();
    const uint16_t vlPOT2 = IIKit.analogReadPot2();
    IIKit.disp.setText(2, ("P1:" + String(vlPOT1)).c_str());
    IIKit.disp.setText(3, ("P2:" + String(vlPOT2)).c_str());    
    wserial::plot("vlPOT1", vlPOT1);
    wserial::plot("vlPOT2", vlPOT2);
}

//Configuração inicial do programa
void setup() {
    jtaskAttachFunc(managerInputFunc, 50000UL); //anexa um função e sua base de tempo para ser executada
    jtaskAttachFunc([](){blinkLEDFunc(def_pin_D1);}, 500000UL);  //anexa um função e sua base de tempo para ser executada
    jtaskAttachFunc([](){blinkLEDFunc(def_pin_D2);}, 1000000UL);  //anexa um função e sua base de tempo para ser executada
}

//Loop principal
void loop() {
  //Monitora os perifericos
  jtaskLoop();  
}