#include "RadiusAdjuster.h"

RadiusAdjuster::RadiusAdjuster(uint8_t servoPin)
  : _servoPin(servoPin), _currentPosition(POS_0), _currentState(DISENGAGED) {}

void RadiusAdjuster::begin() {
    Serial.print("🔗 Attaching servo to pin ");
    Serial.println(_servoPin);
    _servo.attach(_servoPin);
    Serial.print("📍 Servo attached, setting initial position...");
    _updateServoPosition();  // Set initial position
    Serial.println(" Done!");
    Serial.flush();
}

void RadiusAdjuster::setState(Position position, State state) {
    _currentPosition = position;
    _currentState = state;
    _updateServoPosition();
}

void RadiusAdjuster::setPosition(Position position) {
    _currentPosition = position;
    _updateServoPosition();
}

void RadiusAdjuster::engage() {
    _currentState = ENGAGED;
    _updateServoPosition();
}

void RadiusAdjuster::disengage() {
    _currentState = DISENGAGED;
    _updateServoPosition();
}

RadiusAdjuster::Position RadiusAdjuster::getCurrentPosition() const {
    return _currentPosition;
}

RadiusAdjuster::State RadiusAdjuster::getCurrentState() const {
    return _currentState;
}

int RadiusAdjuster::getTotalStateNumber() const {
    return (_currentPosition * 2) + _currentState;  // 0-23
}

// Legacy methods for backward compatibility
void RadiusAdjuster::setAngle(int angle) {
    _servo.write(angle);
}

int RadiusAdjuster::getAngle() {
    return _servo.read();
}

// Utility methods
const char* RadiusAdjuster::getPositionName(Position pos) {
    static const char* names[] = {
        "POS_0", "POS_1", "POS_2", "POS_3", "POS_4", "POS_5",
        "POS_6", "POS_7", "POS_8", "POS_9", "POS_10", "POS_11"
    };
    return (pos >= 0 && pos <= 11) ? names[pos] : "INVALID";
}

const char* RadiusAdjuster::getStateName(State state) {
    return (state == ENGAGED) ? "ENGAGED" : "DISENGAGED";
}

// Private methods
void RadiusAdjuster::_updateServoPosition() {
    int angle = _calculateServoAngle(_currentPosition, _currentState);
    
    // Debug output
    Serial.print("🎯 Setting Servo - Pin ");
    Serial.print(_servoPin);
    Serial.print(", Position: ");
    Serial.print(getPositionName(_currentPosition));
    Serial.print(" (");
    Serial.print(_currentPosition);
    Serial.print("), State: ");
    Serial.print(getStateName(_currentState));
    Serial.print(" -> Angle: ");
    Serial.println(angle);
    Serial.flush();
    
    _servo.write(angle);
}

int RadiusAdjuster::_calculateServoAngle(Position position, State state) {
    // Map 12 positions to servo angles (0-180 degrees)
    // Each position is 15 degrees apart (180/12 = 15)
    int baseAngle = position * 15;  // 0, 15, 30, 45... 165
    
    // Add offset for ENGAGED/DISENGAGED state
    // DISENGAGED: use base angle, ENGAGED: add 7 degrees offset
    int offset = (state == ENGAGED) ? 7 : 0;
    
    return constrain(baseAngle + offset, 0, 180);
}

