#include "RadiusAdjuster.h"

RadiusAdjuster::RadiusAdjuster(uint8_t servoPin1, uint8_t servoPin2)
  : _servoPin1(servoPin1), _servoPin2(servoPin2) {}

void RadiusAdjuster::begin() {
    _servo1.attach(_servoPin1);
    if (_servoPin2 != 255) {
        _servo2.attach(_servoPin2);
    }
}

void RadiusAdjuster::setAngle(int angle) {
    _servo1.write(angle);
    if (_servoPin2 != 255) {
        _servo2.write(angle);
    }
}

int RadiusAdjuster::getAngle() {
    return _servo1.read(); // assume servo2 same
}

