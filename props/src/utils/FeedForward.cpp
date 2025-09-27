#include "FeedForward.h"

// Basic feedforward constructor
FeedForward::FeedForward(float kff) 
  : _kff(kff), _kff_vel(0.0), _kff_accel(0.0), _static_friction(0.0),
    _prev_setpoint(0.0), _prev_velocity(0.0), _prev_time(0),
    _useVelocity(false), _useAcceleration(false), _useStaticFriction(false),
    _initialized(false) {}

// Advanced feedforward constructor
FeedForward::FeedForward(float kff, float kff_vel, float kff_accel, float static_friction)
  : _kff(kff), _kff_vel(kff_vel), _kff_accel(kff_accel), _static_friction(static_friction),
    _prev_setpoint(0.0), _prev_velocity(0.0), _prev_time(0),
    _useVelocity(kff_vel != 0.0), _useAcceleration(kff_accel != 0.0), 
    _useStaticFriction(static_friction != 0.0), _initialized(false) {}

void FeedForward::setGains(float kff, float kff_vel, float kff_accel, float static_friction) {
    _kff = kff;
    _kff_vel = kff_vel;
    _kff_accel = kff_accel;
    _static_friction = static_friction;
    
    _useVelocity = (kff_vel != 0.0);
    _useAcceleration = (kff_accel != 0.0);
    _useStaticFriction = (static_friction != 0.0);
}

// Calculate feedforward with automatic derivative calculation
float FeedForward::calculate(float setpoint, float dt) {
    float output = 0.0;
    
    // Basic feedforward term
    output += _kff * setpoint;
    
    if (_useVelocity || _useAcceleration) {
        if (!_initialized || dt <= 0) {
            // First run or invalid dt, just store values
            _prev_setpoint = setpoint;
            _prev_velocity = 0.0;
            _initialized = true;
            return output;
        }
        
        // Calculate velocity (derivative of setpoint)
        float velocity = _calculateVelocity(setpoint, dt);
        
        if (_useVelocity) {
            output += _kff_vel * velocity;
        }
        
        if (_useAcceleration) {
            // Calculate acceleration (derivative of velocity)
            float acceleration = _calculateAcceleration(velocity, dt);
            output += _kff_accel * acceleration;
        }
        
        // Update state
        _prev_setpoint = setpoint;
        _prev_velocity = velocity;
    }
    
    // Static friction compensation
    if (_useStaticFriction && setpoint != 0.0) {
        output += _static_friction * _sign(setpoint);
    }
    
    _initialized = true;
    return output;
}

// Calculate feedforward with provided velocity and acceleration
float FeedForward::calculate(float setpoint, float velocity, float acceleration) {
    float output = 0.0;
    
    // Basic feedforward term
    output += _kff * setpoint;
    
    // Velocity feedforward
    if (_useVelocity) {
        output += _kff_vel * velocity;
    }
    
    // Acceleration feedforward
    if (_useAcceleration) {
        output += _kff_accel * acceleration;
    }
    
    // Static friction compensation
    if (_useStaticFriction && setpoint != 0.0) {
        output += _static_friction * _sign(setpoint);
    }
    
    return output;
}

void FeedForward::reset() {
    _prev_setpoint = 0.0;
    _prev_velocity = 0.0;
    _prev_time = 0;
    _initialized = false;
}

// Private helper functions
float FeedForward::_calculateVelocity(float setpoint, float dt) {
    if (dt <= 0) return 0.0;
    return (setpoint - _prev_setpoint) / dt;
}

float FeedForward::_calculateAcceleration(float velocity, float dt) {
    if (dt <= 0) return 0.0;
    return (velocity - _prev_velocity) / dt;
}

float FeedForward::_sign(float value) {
    if (value > 0) return 1.0;
    else if (value < 0) return -1.0;
    else return 0.0;
}