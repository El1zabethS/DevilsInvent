/*
 * Example usage of FeedForward class with RPM control
 * 
 * This demonstrates how to combine feedforward with PID control
 * for better RPM tracking performance.
 */

#include "FeedForward.h"
#include "RPM.h"

// Example setup for RPM control with feedforward
class RPMController {
  private:
    RPM* _rpm;
    FeedForward _feedforward;
    
    // PID-like variables (you can integrate with existing PID class)
    float _kp, _ki, _kd;
    float _integral, _prev_error;
    
  public:
    RPMController(RPM* rpm) : _rpm(rpm) {
        // Example gains for a typical DC motor/flywheel system
        // These would need to be tuned for your specific system
        
        float kff = 0.1;        // Base feedforward: ~0.1V per RPM
        float kff_vel = 0.05;   // Velocity feedforward: helps with ramping
        float kff_accel = 0.02; // Acceleration feedforward: helps with quick changes  
        float static_friction = 0.5; // Overcome static friction
        
        _feedforward.setGains(kff, kff_vel, kff_accel, static_friction);
    }
    
    float calculateOutput(float setpoint_rpm, float dt) {
        float current_rpm = _rpm->getRPM();
        
        // Calculate feedforward output (proactive)
        float ff_output = _feedforward.calculate(setpoint_rpm, dt);
        
        // Calculate PID output (reactive to error)
        float error = setpoint_rpm - current_rpm;
        float pid_output = calculatePID(error, dt);
        
        // Combine feedforward and feedback
        float total_output = ff_output + pid_output;
        
        // Clamp output to reasonable range (0-255 for analogWrite, or -255 to 255 for bidirectional)
        return constrain(total_output, 0, 255);
    }
    
  private:
    float calculatePID(float error, float dt) {
        // Basic PID implementation (integrate with your existing PID class)
        _integral += error * dt;
        float derivative = (error - _prev_error) / dt;
        _prev_error = error;
        
        return _kp * error + _ki * _integral + _kd * derivative;
    }
};

/*
 * Tuning Guidelines:
 * 
 * 1. Kff (Basic Feedforward):
 *    - Start with motor's steady-state voltage/RPM ratio
 *    - If motor needs 6V for 1000 RPM, Kff ≈ 6/1000 = 0.006
 *    
 * 2. Kff_vel (Velocity Feedforward):
 *    - Helps with ramping setpoints
 *    - Start with 10-50% of Kff value
 *    - Increase if system is slow to follow ramping setpoints
 *    
 * 3. Kff_accel (Acceleration Feedforward):
 *    - Compensates for motor/load inertia
 *    - Start small (1-10% of Kff)
 *    - Increase if system overshoots on quick setpoint changes
 *    
 * 4. Static Friction:
 *    - Minimum voltage needed to overcome friction
 *    - Usually 5-20% of full scale output
 */