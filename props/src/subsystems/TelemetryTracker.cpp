#include "TelemetryTracker.h"

TelemetryTracker::TelemetryTracker(RPM* rpm, LatchDoor* latch, RadiusAdjuster* adjuster)
  : _rpm(rpm), _latch(latch), _adjuster(adjuster) {}

void TelemetryTracker::setPIDController(PIDController* pidController) {
    _pidController = pidController;
}

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
    
    // Add PID data for graphing if PID controller is available
    // if (_pidController != nullptr) {
    //     json += ",\"pid\":{";
    //     json += "\"setpoint\":" + String(_pidController->getSetpoint(), 2) + ",";
    //     json += "\"measured\":" + String(_pidController->getMeasured(), 2) + ",";
    //     json += "\"error\":" + String(_pidController->getError(), 2) + ",";
    //     json += "\"output\":" + String(_pidController->getOutput(), 2) + ",";
    //     json += "\"integral\":" + String(_pidController->getIntegral(), 3) + ",";
    //     json += "\"derivative\":" + String(_pidController->getDerivative(), 3);
    //     json += "}";
    // }
    
    json += "}";
    return json;
}
