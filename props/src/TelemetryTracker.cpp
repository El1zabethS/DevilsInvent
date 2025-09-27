#include "TelemetryTracker.h"

TelemetryTracker::TelemetryTracker(RPM* rpm, LatchDoor* latch, RadiusAdjuster* adjuster)
  : _rpm(rpm), _latch(latch), _adjuster(adjuster) {}

String TelemetryTracker::getJSON() {
    float rpmVal = _rpm->getRPM();
    
    // For latch, we can’t directly query the angle from Servo library, so track state manually in LatchDoor
    String latchState = (_latch->isOpen()) ? "open" : "closed";

    // For radius adjuster, same issue: track last angle set
    int radiusAngle = _adjuster->getAngle();

    String json = "{";
    json += "\"rpm\":" + String(rpmVal, 2) + ",";
    json += "\"latch\":\"" + latchState + "\",";
    json += "\"radius_angle\":" + String(radiusAngle);
    json += "}";
    return json;
}
