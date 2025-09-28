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
RPM rpm(DC_MOTOR_DIR_PIN, DC_MOTOR_PWM_PIN, DIGITAL_HALL_PIN);
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
int lastDigitalHallValue = -1;
bool hallStartedReading = false;
unsigned long magnetDetections = 0;

void IRAM_ATTR hallISR() {
    rpm.handleInterrupt();
}

void setup() {
    // Add startup delay and immediate feedback
    delay(2000);  // Give Serial Monitor time to connect
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.flush();  // Force immediate output
    
    Serial.println("===========================================");
    Serial.println("🚀 ESP32 SERIAL TEST - CAN YOU SEE THIS?");
    Serial.println("===========================================");
    Serial.flush();
    delay(1000);  // Give time to see this message
    
    Serial.println("ESP32 STARTING UP - SYSTEM INITIALIZING...");
    Serial.println("===========================================");
    Serial.flush();

    Serial.println("Step 1: Initializing hardware components...");
    Serial.flush();
    
    rpm.begin();
    Serial.println("  - RPM sensor initialized");
    Serial.flush();
    
    latch.begin();
    Serial.println("  - Latch door initialized");
    Serial.flush();
    
    adjuster.begin();
    Serial.println("  - Radius adjuster initialized");
    Serial.flush();
    
    // Configure PID controller
    Serial.println("Step 2: Configuring PID controller...");
    Serial.flush();
    
    rpmPID.setSetpoint(rpmSetpoint);
    rpmPID.setOutputLimits(RPM_PID_OUTPUT_MIN, RPM_PID_OUTPUT_MAX);
    Serial.print("  - PID setpoint: ");
    Serial.println(rpmSetpoint);
    Serial.flush();
    
    // Setup hall effect sensor pins
    Serial.println("Step 3: Setting up hall effect sensors...");
    pinMode(DIGITAL_HALL_PIN, INPUT_PULLUP);
    Serial.print("  - Digital hall pin: ");
    Serial.println(DIGITAL_HALL_PIN);
    
    // Enable hall effect interrupt
    attachInterrupt(digitalPinToInterrupt(DIGITAL_HALL_PIN), hallISR, FALLING);
    Serial.println("  - Hall interrupt enabled");
    Serial.flush();  // Force output

    // WiFi setup with timeout
    Serial.println("Step 4: Connecting to WiFi...");
    Serial.print("  - SSID: ");
    Serial.println(WIFI_SSID);
    Serial.flush();
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("  - Connecting");
    Serial.flush();
    
    unsigned long wifiStartTime = millis();
    const unsigned long WIFI_TIMEOUT = 15000; // 15 second timeout
    
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStartTime) < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
        Serial.flush();  // Force each dot to appear
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n  - WiFi Connected! IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\n  - WiFi connection FAILED! Continuing without WiFi...");
        Serial.println("  - Check your WiFi credentials in Constants.h");
    }
    Serial.flush();

    // REST endpoint for telemetry
    Serial.println("Step 5: Starting web server...");
    Serial.flush();
    
    server.on("/telemetry", HTTP_GET, []() {
        server.send(200, "application/json", telemetry.getJSON());
    });
    server.begin();
    Serial.print("  - Web server started on port ");
    Serial.println(WEB_SERVER_PORT);
    Serial.flush();
    
    // Start the 5-second test sequence
    Serial.println("===========================================");
    Serial.println("✅ INITIALIZATION COMPLETE!");
    Serial.println("🚀 Starting 5-second test sequence...");
    Serial.println("===========================================");
    Serial.flush();
    
    testStartTime = millis();
    testRunning = true;
    testCompleted = false;
}

void loop() {
    server.handleClient();
    
    // Add a heartbeat every 5 seconds when not in test mode
    // static unsigned long lastHeartbeat = 0;
    // if (!testRunning && testCompleted && millis() - lastHeartbeat > 5000) {
    //     Serial.println("System running normally - Heartbeat");
    //     lastHeartbeat = millis();
    // }
    
    // Check hall effect sensor and report when it starts detecting magnets
    int digitalHallValue = digitalRead(DIGITAL_HALL_PIN);
    
    // Check if hall sensor values have changed (indicating magnet detection)
    if (digitalHallValue != lastDigitalHallValue) {
        if (!hallStartedReading) {
            Serial.println("✓ Hall effect sensor started detecting magnetic field!");
            hallStartedReading = true;
        }
        
        Serial.print("📡 Hall effect - Digital: ");
        Serial.println(digitalHallValue);
        
        // Count magnet detections (when digital goes LOW, assuming active LOW sensor)
        if (digitalHallValue == LOW && lastDigitalHallValue == HIGH) {
            magnetDetections++;
            Serial.print("🧲 Magnet detected! Count: ");
            Serial.println(magnetDetections);
        }
        
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
                Serial.print("🔄 Test running... ");
                Serial.print(elapsedTime / 1000.0, 1);
                Serial.print("s elapsed, Motor: 150/255, Servo: ");
                Serial.print(servoAngle);
                Serial.println("°");
                lastProgressUpdate = currentTime;
            }
        } else {
            // Test complete - turn everything off
            Serial.println("⏹️  Stopping motor and servo...");
            rpm.stopMotor();
            adjuster.setAngle(0); // Return servo to 0 position
            
            Serial.println("===========================================");
            Serial.println("✅ 5-SECOND TEST COMPLETED SUCCESSFULLY!");
            Serial.println("Motor and servo stopped.");
            Serial.println("System now in monitoring mode.");
            Serial.println("===========================================");
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
