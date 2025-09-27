#ifndef RPM_H
#define RPM_H

#include <Arduino.h>

class RPM {
  public:
    RPM(uint8_t dcMotorPin, uint8_t analogHallPin, uint8_t digitalHallPin, uint16_t pulsesPerRev = 1);
    void begin();
    void handleInterrupt();
    float getRPM();
    
    // Motor control
    void setMotorSpeed(float speed);  // 0-255 for analogWrite
    void stopMotor();

  private:
    uint8_t _dcMotorPin;
    uint8_t _analogHallPin;
    uint8_t _digitalHallPin;
    uint16_t _pulsesPerRev;
    volatile unsigned long _lastPulseTime;
    volatile unsigned long _interval;
};

#endif