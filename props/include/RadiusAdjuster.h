#ifndef RADIUSADJUSTER_H
#define RADIUSADJUSTER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class RadiusAdjuster {
  public:
    // Enum for 12 positions (0-11)
    enum Position {
        POS_0 = 0, POS_1 = 1, POS_2 = 2, POS_3 = 3,
        POS_4 = 4, POS_5 = 5, POS_6 = 6, POS_7 = 7,
        POS_8 = 8, POS_9 = 9, POS_10 = 10, POS_11 = 11
    };
    
    // Enum for state (engaged/disengaged)
    enum State {
        DISENGAGED = 0,
        ENGAGED = 1
    };
    
    RadiusAdjuster(uint8_t servoPin);
    void begin();
    
    // New state-based control methods
    void setState(Position position, State state);
    void setPosition(Position position);
    void engage();
    void disengage();
    
    // Getters
    Position getCurrentPosition() const;
    State getCurrentState() const;
    int getTotalStateNumber() const;  // Returns 0-23 (position * 2 + state)
    
    // Legacy methods (for backward compatibility)
    void setAngle(int angle);
    int getAngle();
    
    // Utility methods
    static const char* getPositionName(Position pos);
    static const char* getStateName(State state);

  private:
    Servo _servo;
    uint8_t _servoPin;
    Position _currentPosition;
    State _currentState;
    
    void _updateServoPosition();
    int _calculateServoAngle(Position position, State state);
};

#endif
