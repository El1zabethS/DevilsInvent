#ifndef LATCHDOOR_H
#define LATCHDOOR_H

#include <Arduino.h>
#include <ESP32Servo.h>

class LatchDoor {
  public:
    LatchDoor(uint8_t servoPin, int openAngle = 90, int closeAngle = 0);
    void begin();
    void open();
    void close();
    bool isOpen();

  private:
    Servo _servo;
    uint8_t _servoPin;
    int _openAngle;
    int _closeAngle;
};

#endif
