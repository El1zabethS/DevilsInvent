#ifndef RPM_H
#define RPM_H

#include <Arduino.h>

class RPM {
  public:
    RPM(uint8_t hallPin, uint16_t pulsesPerRev = 1);
    void begin();
    void handleInterrupt();
    float getRPM();

  private:
    uint8_t _hallPin;
    uint16_t _pulsesPerRev;
    volatile unsigned long _lastPulseTime;
    volatile unsigned long _interval;
};

#endif