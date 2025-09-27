#ifndef TELEMETRYTRACKER_H
#define TELEMETRYTRACKER_H

#include <Arduino.h>
#include "RPM.h"
#include "LatchDoor.h"
#include "RadiusAdjuster.h"
#include "PIDController.h"

class TelemetryTracker {
  public:
    TelemetryTracker(RPM* rpm, LatchDoor* latch, RadiusAdjuster* adjuster, PIDController* pidController = nullptr);
    String getJSON();

    TelemetryTracker(RPM *rpm, LatchDoor *latch, RadiusAdjuster *adjuster);

    // Add PID controller reference
    void setPIDController(PIDController* pidController);

  private:
    RPM* _rpm;
    LatchDoor* _latch;
    RadiusAdjuster* _adjuster;
    PIDController* _pidController;
};

#endif
