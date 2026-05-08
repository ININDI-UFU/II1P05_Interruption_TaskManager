#pragma once
#include <Arduino.h>
#include "util/lasecDebounce.h"

/**
 * @file lasecGPIOKit.h
 * @brief Definições de pinos e inicialização do kit LASEC (ESP32).
 *
 * Uso:
 *   #include "util/lasecGPIOKit.h"
 *
 *   lasecGPIOKit board;
 *
 *   void setup() {
 *       board.begin();
 *   }
 *
 *   void loop() {
 *       board.tick();  // chame a cada ~1 ms para debounce
 *       if (board.push_1.pressed()) { ... }
 *       if (board.rtn_1.held())     { ... }
 *   }
 */

/********** GPIO DEFINITIONS ***********/
constexpr uint8_t def_pin_ADC1    = 39;  ///< GPIO para entrada ADC1. ADC1_CHANNEL_3
constexpr uint8_t def_pin_ADC2    = 36;  ///< GPIO para entrada ADC2. ADC1_CHANNEL_0
constexpr uint8_t def_pin_RTN2    = 35;  ///< GPIO para botão retentivo 2.
constexpr uint8_t def_pin_PUSH1   = 34;  ///< GPIO para botão push 1.
constexpr uint8_t def_pin_PWM     = 33;  ///< GPIO para saída PWM.
constexpr uint8_t def_pin_PUSH2   = 32;  ///< GPIO para botão push 2.
constexpr uint8_t def_pin_RELE    = 27;  ///< GPIO para relé.
constexpr uint8_t def_pin_W4a20_1 = 26;  ///< GPIO para saída 4-20mA 1.
constexpr uint8_t def_pin_DAC1    = 25;  ///< GPIO para saída DAC1.
constexpr uint8_t def_pin_D1      = 23;  ///< GPIO para I/O digital 1.
constexpr uint8_t def_pin_SCL     = 22;  ///< GPIO para SCL do display OLED.
constexpr uint8_t def_pin_SDA     = 21;  ///< GPIO para SDA do display OLED.
constexpr uint8_t def_pin_D2      = 19;  ///< GPIO para I/O digital 2.
constexpr uint8_t def_pin_D3      = 18;  ///< GPIO para I/O digital 3.
///< GPIO15 - ESP_PROG_TDO:6
///< GPIO14 - ESP_PROG_TMS:2
///< GPIO13 - ESP_PROG_TCK:4
///< GPIO12 - ESP_PROG_TDI:8
constexpr uint8_t def_pin_D4      =  4;  ///< GPIO para I/O digital 4.
///< GPIO3  - ESP_COM_TX:3
constexpr uint8_t def_pin_RTN1    =  2;  ///< GPIO para botão retentivo 1.
///< GPIO1  - ESP_COM_RX:5
///< GPIO0  - ESP_COM_BOOT:6
///< ESPEN  - ESP_COM_EN:1

class lasecGPIOKit {
public:
    lasecDebounce rtn_1;
    lasecDebounce rtn_2;
    lasecDebounce push_1;
    lasecDebounce push_2;

    /** @brief Configura todos os pinos e inicializa o debounce. */
    void begin()
    {
        rtn_1.begin (def_pin_RTN1,  INPUT_PULLDOWN, HIGH, 20);
        rtn_2.begin (def_pin_RTN2,  INPUT_PULLDOWN, HIGH, 20);
        push_1.begin(def_pin_PUSH1, INPUT_PULLDOWN, HIGH, 20);
        push_2.begin(def_pin_PUSH2, INPUT_PULLDOWN, HIGH, 20);

        pinMode(def_pin_D1,      OUTPUT);
        pinMode(def_pin_D2,      OUTPUT);
        pinMode(def_pin_D3,      OUTPUT);
        pinMode(def_pin_D4,      OUTPUT);
        pinMode(def_pin_PWM,     OUTPUT);

        // pinMode(def_pin_DAC1,    ANALOG);
        // pinMode(def_pin_ADC1,    ANALOG);
        
        pinMode(def_pin_RELE,    OUTPUT);
        pinMode(def_pin_W4a20_1, OUTPUT);

        digitalWrite(def_pin_D1,      LOW);
        digitalWrite(def_pin_D2,      LOW);
        digitalWrite(def_pin_D3,      LOW);
        digitalWrite(def_pin_D4,      LOW);
        digitalWrite(def_pin_RELE,    LOW);
        analogWrite (def_pin_PWM,     0);
        analogWrite (def_pin_DAC1,    0);
        analogWrite (def_pin_W4a20_1, 0);
    }

    /**
     * @brief Processa uma amostra de debounce em todos os botões.
     *        Chamar a cada ~1 ms (em loop() ou jtask periódica).
     */
    void tick()
    {
        rtn_1.tick();
        rtn_2.tick();
        push_1.tick();
        push_2.tick();
    }
};
