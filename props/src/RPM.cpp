#include "RPM.h"

RPM::RPM(uint8_t hallPin, uint16_t pulsesPerRev)
  : _hallPin(hallPin), _pulsesPerRev(pulsesPerRev), _lastPulseTime(0), _interval(0) {}

void RPM::begin() {
    pinMode(_hallPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_hallPin), []{
        // Static lambda needed for ISR
    }, FALLING);
}

void RPM::handleInterrupt() {
    unsigned long now = micros();
    _interval = now - _lastPulseTime;
    _lastPulseTime = now;
}

float RPM::getRPM() {
    if (_interval == 0) return 0.0;
    return (60.0 * 1000000.0) / (_interval * _pulsesPerRev);
}
