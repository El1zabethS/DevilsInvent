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
RadiusAdjuster adjuster(SERVO_RADIUS_1, SERVO_RADIUS_2);
PIDController rpmPID(RPM_PID_KP, RPM_PID_KI, RPM_PID_KD);
TelemetryTracker telemetry(&rpm, &latch, &adjuster, &rpmPID);

WebServer server(WEB_SERVER_PORT);

// PID control variables
float rpmSetpoint = DEFAULT_RPM_SETPOINT;
unsigned long lastPIDUpdate = 0;
const unsigned long PID_UPDATE_INTERVAL = 20; // 50Hz update rate

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
}

void loop() {
    server.handleClient();
    
    // PID control loop
    unsigned long currentTime = millis();
    if (currentTime - lastPIDUpdate >= PID_UPDATE_INTERVAL) {
        // Get current RPM measurement
        float currentRPM = rpm.getRPM();
        
        // Calculate PID output (update method handles timing internally)
        float pidOutput = rpmPID.update(currentRPM);
        
        // Apply output to motor
        rpm.setMotorSpeed(pidOutput);
        
        // Debug output (remove in production)
        Serial.print("Setpoint: ");
        Serial.print(rpmSetpoint);
        Serial.print(" RPM, Measured: ");
        Serial.print(currentRPM);
        Serial.print(" RPM, Output: ");
        Serial.println(pidOutput);
        
        lastPIDUpdate = currentTime;
    }
}
