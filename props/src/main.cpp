#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "RPM.h"
#include "LatchDoor.h"
#include "RadiusAdjuster.h"
#include "TelemetryTracker.h"

// Pin assignments
const uint8_t HALL_PIN = 34;
const uint8_t SERVO_LATCH = 25;
const uint8_t SERVO_RADIUS_1 = 26;
const uint8_t SERVO_RADIUS_2 = 27;

// WiFi creds
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASS";

// Objects
RPM rpm(HALL_PIN);
LatchDoor latch(SERVO_LATCH);
RadiusAdjuster adjuster(SERVO_RADIUS_1, SERVO_RADIUS_2);
TelemetryTracker telemetry(&rpm, &latch, &adjuster);

WebServer server(80);

void IRAM_ATTR hallISR() {
    rpm.handleInterrupt();
}

void setup() {
    Serial.begin(115200);

    rpm.begin();
    latch.begin();
    adjuster.begin();
    attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, FALLING);

    // WiFi setup
    WiFi.begin(ssid, password);
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
}
