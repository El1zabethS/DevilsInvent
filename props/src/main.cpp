#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "Constants.h"
#include "RPM.h"
#include "LatchDoor.h"
#include "RadiusAdjuster.h"
#include "TelemetryTracker.h"
#include "PIDController.h"

// Objects
RPM rpm(DC_MOTOR_RPM_PIN, ANALOG_HALL_PIN, DIGITAL_HALL_PIN);
LatchDoor latch(SERVO_LATCH);
RadiusAdjuster adjuster(SERVO_RADIUS_1);
PIDController rpmPID(RPM_PID_KP, RPM_PID_KI, RPM_PID_KD);
TelemetryTracker telemetry(&rpm, &latch, &adjuster, &rpmPID);

WebServer server(WEB_SERVER_PORT);

// PID control variables
float rpmSetpoint = DEFAULT_RPM_SETPOINT;
unsigned long lastPIDUpdate = 0;
const unsigned long PID_UPDATE_INTERVAL = 20; // 50Hz update rate

// Test sequence variables
unsigned long testStartTime = 0;
const unsigned long TEST_DURATION = 5000; // 5 seconds in milliseconds
bool testRunning = false;
bool testCompleted = false;

// Hall effect sensor variables
int lastAnalogHallValue = -1;
int lastDigitalHallValue = -1;
bool hallStartedReading = false;
unsigned long magnetDetections = 0;

void IRAM_ATTR hallISR() {
    rpm.handleInterrupt();
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    rpm.begin();
    latch.begin();
    adjuster.begin();
    
    // Configure PID controller
    rpmPID.setSetpoint(rpmSetpoint);
    rpmPID.setOutputLimits(RPM_PID_OUTPUT_MIN, RPM_PID_OUTPUT_MAX);
    
    // Setup hall effect sensor pins
    pinMode(ANALOG_HALL_PIN, INPUT);
    pinMode(DIGITAL_HALL_PIN, INPUT_PULLUP);
    
    // attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, FALLING);

    // WiFi setup
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

    // REST endpoint for telemetry
    server.on("/telemetry", HTTP_GET, []() {
        server.send(200, "application/json", telemetry.getJSON());
    });
    server.begin();
    
    // Start the 5-second test sequence
    Serial.println("Starting 5-second test sequence...");
    testStartTime = millis();
    testRunning = true;
    testCompleted = false;
}

void loop() {
    server.handleClient();
    
    // Check hall effect sensor and report when it starts detecting magnets
    int analogHallValue = analogRead(ANALOG_HALL_PIN);
    int digitalHallValue = digitalRead(DIGITAL_HALL_PIN);
    
    // Check if hall sensor values have changed (indicating magnet detection)
    if (analogHallValue != lastAnalogHallValue || digitalHallValue != lastDigitalHallValue) {
        if (!hallStartedReading) {
            Serial.println("Hall effect sensor started detecting magnetic field!");
            hallStartedReading = true;
        }
        
        // Report significant changes in analog reading (threshold to avoid noise)
        if (abs(analogHallValue - lastAnalogHallValue) > 50) {
            Serial.print("Hall effect - Analog: ");
            Serial.print(analogHallValue);
            Serial.print(", Digital: ");
            Serial.println(digitalHallValue);
            
            // Count magnet detections (when digital goes LOW, assuming active LOW sensor)
            if (digitalHallValue == LOW && lastDigitalHallValue == HIGH) {
                magnetDetections++;
                Serial.print("Magnet detected! Count: ");
                Serial.println(magnetDetections);
            }
        }
        
        lastAnalogHallValue = analogHallValue;
        lastDigitalHallValue = digitalHallValue;
    }
    
    // 5-second test sequence
    if (testRunning && !testCompleted) {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - testStartTime;
        
        if (elapsedTime < TEST_DURATION) {
            // Run motor at moderate speed (adjust as needed)
            rpm.setMotorSpeed(150); // 150/255 = ~60% speed
            
            // Move servo through a range (0-180 degrees over 5 seconds)
            int servoAngle = map(elapsedTime, 0, TEST_DURATION, 0, 180);
            adjuster.setAngle(servoAngle);
            
            // Print progress every 500ms
            static unsigned long lastProgressUpdate = 0;
            if (currentTime - lastProgressUpdate >= 500) {
                Serial.print("Test running... ");
                Serial.print(elapsedTime / 1000.0, 1);
                Serial.print("s elapsed, Motor speed: 150, Servo angle: ");
                Serial.println(servoAngle);
                lastProgressUpdate = currentTime;
            }
        } else {
            // Test complete - turn everything off
            rpm.stopMotor();
            adjuster.setAngle(0); // Return servo to 0 position
            
            Serial.println("5-second test completed! Motor and servo stopped.");
            testRunning = false;
            testCompleted = true;
        }
    }
    
    // Optional: Uncomment below for continuous PID control after test
    // PID control loop
    // if (testCompleted) {
    //     unsigned long currentTime = millis();
    //     if (currentTime - lastPIDUpdate >= PID_UPDATE_INTERVAL) {
    //         // Get current RPM measurement
    //         float currentRPM = rpm.getRPM();
            
    //         // Calculate PID output (update method handles timing internally)
    //         float pidOutput = rpmPID.update(currentRPM);
            
    //         // Apply output to motor
    //         rpm.setMotorSpeed(pidOutput);
            
    //         // Debug output (remove in production)
    //         Serial.print("Setpoint: ");
    //         Serial.print(rpmSetpoint);
    //         Serial.print(" RPM, Measured: ");
    //         Serial.print(currentRPM);
    //         Serial.print(" RPM, Output: ");
    //         Serial.println(pidOutput);
            
    //         lastPIDUpdate = currentTime;
    //     }
    // }
}
