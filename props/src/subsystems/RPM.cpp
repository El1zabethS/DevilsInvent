#include "RPM.h"

RPM::RPM(uint8_t dcMotorDirPin, uint8_t dcMotorPwmPin, uint8_t digitalHallPin, uint16_t pulsesPerRev)
  : _dcMotorDirPin(dcMotorDirPin), _dcMotorPwmPin(dcMotorPwmPin), _digitalHallPin(digitalHallPin), _pulsesPerRev(pulsesPerRev), _lastPulseTime(0), _interval(0) {}

void RPM::begin() {
    Serial.print("🔌 Setting up motor pins - Dir: ");
    Serial.print(_dcMotorDirPin);
    Serial.print(", PWM: ");
    Serial.println(_dcMotorPwmPin);
    
    pinMode(_dcMotorDirPin, OUTPUT);
    pinMode(_dcMotorPwmPin, OUTPUT);
    pinMode(_digitalHallPin, INPUT_PULLUP);
    
    // Set initial motor state (stopped, forward direction)
    digitalWrite(_dcMotorDirPin, HIGH);  // Forward direction
    analogWrite(_dcMotorPwmPin, 0);      // Stopped
    
    Serial.println("⚡ Motor pins configured, motor stopped in forward direction");
    Serial.flush();
    
    attachInterrupt(digitalPinToInterrupt(_digitalHallPin), []{
        // Static lambda needed for ISR
    }, FALLING);
}

void RPM::handleInterrupt() {
    unsigned long now = micros();
    _interval = now - _lastPulseTime;
    _lastPulseTime = now;
}

float RPM::getRPM() {
    if (_interval == 0) return 0.0;
    return (60.0 * 1000000.0) / (_interval * _pulsesPerRev);
}

void RPM::setMotorSpeed(float speed) {
    // Constrain speed to valid range
    speed = constrain(speed, 0, 255);
    
    // Debug output
    Serial.print("Setting Motor - Dir Pin ");
    Serial.print(_dcMotorDirPin);
    Serial.print(" = HIGH, PWM Pin ");
    Serial.print(_dcMotorPwmPin);
    Serial.print(" = ");
    Serial.println((int)speed);
    
    // Set direction (always forward for now)
    digitalWrite(_dcMotorDirPin, HIGH);
    
    // Set PWM speed
    analogWrite(_dcMotorPwmPin, (int)speed);
    Serial.flush();
}

void RPM::stopMotor() {
    analogWrite(_dcMotorPwmPin, 0);
}
