#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ========================================
// PIN ASSIGNMENTS
// ========================================

// RPM Sensor and Motor Control Pins
const uint8_t DC_MOTOR_DIR_PIN = 21;  // Motor direction control
const uint8_t DC_MOTOR_PWM_PIN = 18;  // Motor speed control (PWM)
const uint8_t DIGITAL_HALL_PIN = 23;

// Servo Control Pins
const uint8_t SERVO_LATCH = 32;
const uint8_t SERVO_RADIUS_1 = 22;

// ========================================
// NETWORK CONFIGURATION
// ========================================

// WiFi Credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASS";

// Web Server Configuration
const uint16_t WEB_SERVER_PORT = 80;

// ========================================
// SYSTEM PARAMETERS
// ========================================

// Serial Communication
const uint32_t SERIAL_BAUD_RATE = 115200;

// RPM Configuration
const uint16_t DEFAULT_PULSES_PER_REV = 1;
const float DEFAULT_RPM_SETPOINT = 1500.0;  // Target RPM (replaceable)

// PID Controller Parameters
const float RPM_PID_KP = 0.0;     // Proportional gain (tune this first)
const float RPM_PID_KI = 0.0;     // Integral gain (add after Kp is tuned)
const float RPM_PID_KD = 0.0;     // Derivative gain (add last for stability)

// PID Output Limits (for PWM or motor driver)
const float RPM_PID_OUTPUT_MIN = 0.0;     // Minimum output (0 = motor off)
const float RPM_PID_OUTPUT_MAX = 255.0;   // Maximum output (255 = full speed for analogWrite)

// Servo Configuration
const uint16_t SERVO_MIN_PULSE_WIDTH = 500;   // microseconds
const uint16_t SERVO_MAX_PULSE_WIDTH = 2500;  // microseconds
const uint16_t SERVO_DEFAULT_FREQUENCY = 50;  // Hz

// ========================================
// TIMING CONSTANTS
// ========================================

// Update intervals (milliseconds)
const uint32_t TELEMETRY_UPDATE_INTERVAL = 100;
const uint32_t RPM_CALCULATION_INTERVAL = 50;
const uint32_t SERVO_UPDATE_INTERVAL = 20;

// Timeout values (milliseconds)
const uint32_t WIFI_CONNECTION_TIMEOUT = 30000;
const uint32_t SENSOR_TIMEOUT = 5000;

#endif