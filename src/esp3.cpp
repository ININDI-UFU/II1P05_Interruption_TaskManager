// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html
#include "IIKit.h"
#include "util/dinDebounce.h"

// ---------- Funções auxiliares ----------

// Altera o estado de um LED
void blinkLEDFunc(uint8_t pin) {
    digitalWrite(pin, !digitalRead(pin));
}

// Lê os POTs e atualiza o display
void managerInputFunc(void) {
    const uint16_t vlPOT1 = IIKit.analogReadPot1();
    const uint16_t vlPOT2 = IIKit.analogReadPot2();
    IIKit.disp.setText(2, ("P1:" + String(vlPOT1)).c_str());
    IIKit.disp.setText(3, ("P2:" + String(vlPOT2)).c_str());
    wserial::plot("vlPOT1", vlPOT1);
    wserial::plot("vlPOT2", vlPOT2);
}

// ---------- Tasks FreeRTOS ----------

// Task: pisca LED D1 a cada 500 ms
void taskBlinkD1(void* pvParameters) {
    for (;;) {
        blinkLEDFunc(def_pin_D1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Task: pisca LED D2 a cada 1000 ms
void taskBlinkD2(void* pvParameters) {
    for (;;) {
        blinkLEDFunc(def_pin_D2);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task: lê entradas analógicas a cada 50 ms
void taskManagerInput(void* pvParameters) {
    for (;;) {
        managerInputFunc();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ---------- Setup ----------
void setup() {
    IIKit.setup();
    // Cria as tasks no núcleo 1 (núcleo do Arduino por padrão)
    xTaskCreatePinnedToCore(taskBlinkD1,      "BlinkD1",    2048, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(taskBlinkD2,      "BlinkD2",    2048, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(taskManagerInput, "ManagerPOT", 4096, nullptr, 1, nullptr, 1);
}

// ---------- Loop principal ----------
void loop() {
    IIKit.loop();         // Monitora os periféricos do kit
}
