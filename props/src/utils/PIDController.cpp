#include "PIDController.h"

PIDController::PIDController(double P, double I, double D) 
    : kP(P), kI(I), kD(D), setpoint(0.0), measuredValue(0.0), tolerance(0.0), 
      previousError(0.0), integral(0.0), derivative(0.0), output(0.0), lastTime(0),
      firstUpdate(true), _min_output(-255.0), _max_output(255.0), _output_limits_enabled(true) {}

void PIDController::setTolerance(double tol) {
    tolerance = tol;
}

void PIDController::setSetpoint(double target) {
    setpoint = target;
    integral = 0.0;  // Reset integral when setpoint changes
    firstUpdate = true;
}

bool PIDController::atSetpoint(double currentValue) {
    return abs(currentValue - setpoint) <= tolerance;
}
double PIDController::update(double currentValue) {
    measuredValue = currentValue;  // Store for telemetry
    unsigned long currentTime = millis();
    
    if (firstUpdate) {
        lastTime = currentTime;
        firstUpdate = false;
        previousError = setpoint - currentValue;
        output = kP * previousError;
        
        // Apply output limits
        if (_output_limits_enabled) {
            output = _constrain(output, _min_output, _max_output);
        }
        
        return output;
    }

    double dt = (currentTime - lastTime) / 1000.0;  // Convert to seconds
    if (dt <= 0) return output;  // Avoid division by zero
    
    double error = setpoint - currentValue;

    // Proportional term
    double proportionalTerm = kP * error;

    // Integral term
    integral += error * dt;
    double integralTerm = kI * integral;

    // Derivative term
    derivative = (error - previousError) / dt;
    double derivativeTerm = kD * derivative;

    // Calculate total output
    output = proportionalTerm + integralTerm + derivativeTerm;
    
    // Apply output limits
    if (_output_limits_enabled) {
        output = _constrain(output, _min_output, _max_output);
    }

    // Update for next iteration
    previousError = error;
    lastTime = currentTime;

    return output;
}

void PIDController::reset() {
    integral = 0.0;
    derivative = 0.0;
    output = 0.0;
    previousError = 0.0;
    firstUpdate = true;
}

void PIDController::setGains(double kp, double ki, double kd) {
    kP = kp;
    kI = ki;
    kD = kd;
}

void PIDController::setOutputLimits(double min_output, double max_output) {
    _min_output = min_output;
    _max_output = max_output;
    _output_limits_enabled = true;
    
    // Clamp current output if it's outside new limits
    output = _constrain(output, min_output, max_output);
}

double PIDController::_constrain(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}
