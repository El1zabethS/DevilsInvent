#ifndef TELEMETRYTRACKER_H
#define TELEMETRYTRACKER_H

#include <Arduino.h>
#include "RPM.h"
#include "LatchDoor.h"
#include "RadiusAdjuster.h"

class TelemetryTracker {
  public:
    TelemetryTracker(RPM* rpm, LatchDoor* latch, RadiusAdjuster* adjuster);
    String getJSON();

  private:
    RPM* _rpm;
    LatchDoor* _latch;
    RadiusAdjuster* _adjuster;
};

#endif
