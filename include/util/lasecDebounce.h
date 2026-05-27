#pragma once
#include <Arduino.h>

/**
 * @file lasecDebounce.h
 * @brief Debounce simples baseado em tempo.
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
        pinMode(_pin, mode);

        _lastRawState = isActive();
        _stableState  = _lastRawState;
        _lastChangeMs = millis();
    }

    Event tick()
    {
        const bool rawState = isActive();
        const uint32_t now = millis();

        if (rawState != _lastRawState) {
            _lastRawState = rawState;
            _lastChangeMs = now;
        }

        if (rawState != _stableState && (now - _lastChangeMs) >= _window) {
            _stableState = rawState;

            if (_stableState) {
                return Pressed;
            }
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
    bool isActive() const
    {
        return digitalRead(_pin) == _activeLevel;
    }

    uint8_t  _pin          = 0;
    uint8_t  _activeLevel  = LOW;
    uint8_t  _window       = 20;
    bool     _lastRawState = false;
    bool     _stableState  = false;
    uint32_t _lastChangeMs = 0;
};
