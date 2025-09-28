/*
 * RadiusAdjuster Usage Examples - Updated with ENGAGED/DISENGAGED states
 * 
 * This file demonstrates how to use the 24-state RadiusAdjuster
 */

void exampleUsage() {
    RadiusAdjuster adjuster(SERVO_RADIUS_1);
    adjuster.begin();
    
    // Method 1: Set position and state together
    adjuster.setState(RadiusAdjuster::POS_5, RadiusAdjuster::ENGAGED);     // State 11 (5*2+1)
    adjuster.setState(RadiusAdjuster::POS_3, RadiusAdjuster::DISENGAGED);  // State 6 (3*2+0)
    
    // Method 2: Set position then engage/disengage
    adjuster.setPosition(RadiusAdjuster::POS_8);  // Go to position 8
    adjuster.engage();                            // Engage (state 17: 8*2+1)
    adjuster.disengage();                         // Disengage (state 16: 8*2+0)
    
    // Method 3: Query current state
    RadiusAdjuster::Position currentPos = adjuster.getCurrentPosition();
    RadiusAdjuster::State currentState = adjuster.getCurrentState();
    int totalState = adjuster.getTotalStateNumber();  // 0-23
    
    Serial.print("Position: ");
    Serial.print(RadiusAdjuster::getPositionName(currentPos));
    Serial.print(", State: ");
    Serial.print(RadiusAdjuster::getStateName(currentState));
    Serial.print(", Total State: ");
    Serial.println(totalState);
    
    // Example: Cycle through all 24 states
    for (int pos = 0; pos < 12; pos++) {
        for (int state = 0; state < 2; state++) {
            RadiusAdjuster::Position position = (RadiusAdjuster::Position)pos;
            RadiusAdjuster::State stateEnum = (RadiusAdjuster::State)state;
            
            adjuster.setState(position, stateEnum);
            
            Serial.print("State ");
            Serial.print(pos * 2 + state);
            Serial.print(": ");
            Serial.print(RadiusAdjuster::getPositionName(position));
            Serial.print(" ");
            Serial.println(RadiusAdjuster::getStateName(stateEnum));
            
            delay(500);  // Wait 500ms between states
        }
    }
}

/*
 * State Mapping:
 * 
 * State 0:  POS_0  DISENGAGED  (Servo angle: 0°)
 * State 1:  POS_0  ENGAGED     (Servo angle: 7°)
 * State 2:  POS_1  DISENGAGED  (Servo angle: 15°)
 * State 3:  POS_1  ENGAGED     (Servo angle: 22°)
 * State 4:  POS_2  DISENGAGED  (Servo angle: 30°)
 * State 5:  POS_2  ENGAGED     (Servo angle: 37°)
 * ...
 * State 22: POS_11 DISENGAGED  (Servo angle: 165°)
 * State 23: POS_11 ENGAGED     (Servo angle: 172°)
 */