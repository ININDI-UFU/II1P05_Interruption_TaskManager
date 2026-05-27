#pragma once
#include <Arduino.h>

/**
 * @file lasecDebounce.h
 * @brief Debounce simples por integrador.
 *
 * Uso:
 *   lasecDebounce btn;
 *   btn.begin(PIN, INPUT_PULLUP, LOW, 20);
 *
 *   void loop() {
 *       btn.tick([](lasecDebounce::Event event) {
 *           if (event == lasecDebounce::Pressed)  Serial.println("pressed");
 *           if (event == lasecDebounce::Released) Serial.println("released");
 *           if (event == lasecDebounce::Held)     Serial.println("held");
 *       });
 *   }
 */
class lasecDebounce {
public:
    enum Event : uint8_t {
        None,
        Pressed,
        Released,
        Held
    };

    lasecDebounce() = default;

    void begin(uint8_t pin,
               uint8_t mode        = INPUT_PULLUP,
               uint8_t activeLevel = LOW,
               uint8_t windowMs    = 20)
    {
        _pin         = pin;
        _activeLevel = activeLevel;
        _window      = (windowMs < 2) ? 2 : windowMs;
        _integrator  = 0;
        _stableState = false;
        pinMode(_pin, mode);
    }

    Event tick()
    {
        if (digitalRead(_pin) == _activeLevel) {
            if (_integrator < _window) ++_integrator;
        } else {
            if (_integrator > 0) --_integrator;
        }

        if (!_stableState && _integrator >= _window) {
            _stableState = true;
            return Pressed;
        }

        if (_stableState && _integrator == 0) {
            _stableState = false;
            return Released;
        }

        return _stableState ? Held : None;
    }

    template <typename Callback>
    void tick(Callback callback)
    {
        const Event event = tick();
        if (event != None) {
            callback(event);
        }
    }

private:
    uint8_t _pin         = 0;
    uint8_t _activeLevel = LOW;
    uint8_t _window      = 20;
    uint8_t _integrator  = 0;
    bool    _stableState = false;
};
