#include "LatchDoor.h"

LatchDoor::LatchDoor(uint8_t servoPin, int openAngle, int closeAngle)
  : _servoPin(servoPin), _openAngle(openAngle), _closeAngle(closeAngle) {}

void LatchDoor::begin() {
    _servo.attach(_servoPin);
    close();
}

void LatchDoor::open() {
    _servo.write(_openAngle);
}

void LatchDoor::close() {
    _servo.write(_closeAngle);
}

bool LatchDoor::isOpen() {
    return (_servo.read() == _openAngle);
}

