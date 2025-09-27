#ifndef RADIUSADJUSTER_H
#define RADIUSADJUSTER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class RadiusAdjuster {
  public:
    RadiusAdjuster(uint8_t servoPin1, uint8_t servoPin2 = 255);
    void begin();
    void setAngle(int angle);
    int getAngle();

  private:
    Servo _servo1;
    Servo _servo2;
    uint8_t _servoPin1;
    uint8_t _servoPin2;
};

#endif
