#ifndef FEEDFORWARD_H
#define FEEDFORWARD_H

#include <Arduino.h>

class FeedForward {
  public:
    // Constructor for basic feedforward
    FeedForward(float kff = 0.0);
    
    // Constructor for advanced feedforward with velocity and acceleration terms
    FeedForward(float kff, float kff_vel, float kff_accel = 0.0, float static_friction = 0.0);
    
    // Set gains
    void setGains(float kff, float kff_vel = 0.0, float kff_accel = 0.0, float static_friction = 0.0);
    
    // Calculate feedforward output
    float calculate(float setpoint, float dt);
    float calculate(float setpoint, float velocity, float acceleration);
    
    // Reset internal state
    void reset();
    
    // Enable/disable components
    void enableVelocityFF(bool enable) { _useVelocity = enable; }
    void enableAccelerationFF(bool enable) { _useAcceleration = enable; }
    void enableStaticFriction(bool enable) { _useStaticFriction = enable; }

  private:
    // Gains
    float _kff;              // Steady-state feedforward gain
    float _kff_vel;          // Velocity feedforward gain  
    float _kff_accel;        // Acceleration feedforward gain
    float _static_friction;  // Static friction compensation
    
    // State tracking for derivative calculation
    float _prev_setpoint;
    float _prev_velocity;
    unsigned long _prev_time;
    
    // Enable flags
    bool _useVelocity;
    bool _useAcceleration;
    bool _useStaticFriction;
    bool _initialized;
    
    // Helper functions
    float _calculateVelocity(float setpoint, float dt);
    float _calculateAcceleration(float velocity, float dt);
    float _sign(float value);
};

#endif