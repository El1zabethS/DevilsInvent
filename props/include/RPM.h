#ifndef RPM_H
#define RPM_H

#include <Arduino.h>

class RPM {
  public:
    RPM(uint8_t dcMotorDirPin, uint8_t dcMotorPwmPin, uint8_t digitalHallPin, uint16_t pulsesPerRev = 1);
    void begin();
    void handleInterrupt();
    float getRPM();
    
    // Motor control
    void setMotorSpeed(float speed);  // 0-255 for PWM speed, always forward direction
    void stopMotor();

  private:
    uint8_t _dcMotorDirPin;   // Direction control pin
    uint8_t _dcMotorPwmPin;   // PWM speed control pin
    uint8_t _digitalHallPin;
    uint16_t _pulsesPerRev;
    volatile unsigned long _lastPulseTime;
    volatile unsigned long _interval;
};

#endif