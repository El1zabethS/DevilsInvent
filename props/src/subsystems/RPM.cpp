#include "RPM.h"

RPM::RPM(uint8_t dcMotorPin, uint8_t analogHallPin, uint8_t digitalHallPin, uint16_t pulsesPerRev)
  : _dcMotorPin(dcMotorPin), _analogHallPin(analogHallPin), _digitalHallPin(digitalHallPin), _pulsesPerRev(pulsesPerRev), _lastPulseTime(0), _interval(0) {}

void RPM::begin() {
    pinMode(_dcMotorPin, OUTPUT);
    pinMode(_analogHallPin, INPUT);
    pinMode(_digitalHallPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_digitalHallPin), []{
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

void RPM::setMotorSpeed(float speed) {
    // Constrain speed to valid range
    speed = constrain(speed, 0, 255);
    analogWrite(_dcMotorPin, (int)speed);
}

void RPM::stopMotor() {
    analogWrite(_dcMotorPin, 0);
}
