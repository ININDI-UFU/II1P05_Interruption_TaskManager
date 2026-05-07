// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
#include "IIKit.h"

// ---------- Flags setadas pela ISR do timer ----------
// volatile: impede que o compilador otimize o acesso a essas variáveis
volatile bool flagBlink = false;
volatile bool flagInput = false;

// ---------- Funções auxiliares ----------

void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

void managerInputFunc(void) {
    const uint16_t vlPOT1 = IIKit.analogReadPot1();
    const uint16_t vlPOT2 = IIKit.analogReadPot2();
    IIKit.disp.setText(2, ("P1:" + String(vlPOT1) + "  P2:" + String(vlPOT2)).c_str());    
    wserial::plot("vlPOT1", vlPOT1);
    wserial::plot("vlPOT2", vlPOT2);
}

// ---------- ISR do timer (roda a cada 50 ms) ----------
// IRAM_ATTR: mantém a função na RAM para execução rápida
hw_timer_t* timer = nullptr;
uint8_t contadorBlink = 0; // conta quantas vezes o timer disparou

void IRAM_ATTR onTimer() {
    contadorBlink++;
    flagInput = true;           // sinaliza leitura dos POTs (a cada 50 ms)
    if (contadorBlink >= 10) {  // 10 x 50 ms = 500 ms
        contadorBlink = 0;
        flagBlink = true;       // sinaliza pisca-LED
    }
}

// ---------- Setup ----------
void setup() {
    IIKit.setup();
    timer = timerBegin(1000000); // frequência do timer: 1 MHz (1 tick = 1 µs)
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, 50000, true, 0); // dispara a cada 50.000 µs = 50 ms
}

// ---------- Loop principal ----------
void loop() {
    IIKit.loop();
    // Verifica as flags setadas pela ISR — processamento fora da interrupção
    if (flagInput) {
        flagInput = false;
        managerInputFunc();
    }
    if (flagBlink) {
        flagBlink = false;
        blinkLEDFunc(def_pin_D1);
    }
}