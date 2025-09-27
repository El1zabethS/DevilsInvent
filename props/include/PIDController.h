#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <Arduino.h>

class PIDController {
  private:
    double kP, kI, kD;
    double setpoint;
    double measuredValue;
    double tolerance;
    double previousError;
    double integral;
    double derivative;
    double output;
    unsigned long lastTime;
    bool firstUpdate;

  public:
    PIDController(double P, double I, double D);
    
    void setTolerance(double tol);
    void setSetpoint(double target);
    bool atSetpoint(double currentValue);
    double update(double currentValue);
    void reset();
    
    // Setters for PID gains
    void setGains(double kp, double ki, double kd);
    void setKp(double kp) { kP = kp; }
    void setKi(double ki) { kI = ki; }
    void setKd(double kd) { kD = kd; }
    
    // Getters for PID gains
    double getKp() const { return kP; }
    double getKi() const { return kI; }
    double getKd() const { return kD; }
    
    // Output limits
    void setOutputLimits(double min_output, double max_output);
    
    // Getters for telemetry and graphing
    double getSetpoint() const { return setpoint; }
    double getMeasured() const { return measuredValue; }
    double getError() const { return setpoint - measuredValue; }
    double getIntegral() const { return integral; }
    double getDerivative() const { return derivative; }
    double getOutput() const { return output; }
    
  private:
    double _min_output;
    double _max_output;
    bool _output_limits_enabled;
    double _constrain(double value, double min_val, double max_val);
};

#endif