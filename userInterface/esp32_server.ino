/*
 * Honeycomb Devils Invent 2025 - ESP32 WebSocket Server
 * 
 * This sketch creates a web server on the ESP32 that:
 * 1. Serves the HTML/CSS/JS dashboard files
 * 2. Provides WebSocket connection for real-time data
 * 3. Simulates robot telemetry data
 * 
 * Hardware: ESP32 Dev Board
 * Libraries needed: WebServer, WebSocketsServer, SPIFFS (or LittleFS)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Wi-Fi credentials - CHANGE THESE!
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Server configuration
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
const int LED_PIN = 2; // Built-in LED

// Robot data structure
struct RobotData {
  float speed;
  float distance;
  float angle;
  float temperature;
  float battery;
  int propeller_rpm;
  int propeller_thrust;
  String status;
  unsigned long timestamp;
};

RobotData robotData;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 1000; // Update every 1 second

// Function prototypes
void handleRoot();
void handleNotFound();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void updateRobotData();
void sendTelemetryData();
void setupSPIFFS();

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Starting Honeycomb Devils Invent 2025 Dashboard...");
  
  // Initialize SPIFFS for file serving
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize robot data
  robotData.speed = 0.0;
  robotData.distance = 0.0;
  robotData.angle = 0.0;
  robotData.temperature = 25.0;
  robotData.battery = 12.4;
  robotData.propeller_rpm = 1250;
  robotData.propeller_thrust = 85;
  robotData.status = "Active";
  robotData.timestamp = millis();
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  
  // Start servers
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("HTTP server started on port 80");
  Serial.println("WebSocket server started on port 81");
  Serial.println("Dashboard available at: http://" + WiFi.localIP().toString());
  
  digitalWrite(LED_PIN, HIGH); // Indicate server is running
}

void loop() {
  server.handleClient();
  webSocket.loop();
  
  // Update robot data and send to clients
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    updateRobotData();
    sendTelemetryData();
    lastUpdate = millis();
  }
  
  delay(10); // Small delay to prevent watchdog reset
}

void handleRoot() {
  // Serve the main dashboard HTML file
  File file = SPIFFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Dashboard not found. Please upload files to SPIFFS.");
  }
}

void handleNotFound() {
  String path = server.uri();
  String contentType = "text/plain";
  
  // Determine content type based on file extension
  if (path.endsWith(".css")) {
    contentType = "text/css";
  } else if (path.endsWith(".js")) {
    contentType = "application/javascript";
  } else if (path.endsWith(".json")) {
    contentType = "application/json";
  }
  
  // Try to serve the requested file
  File file = SPIFFS.open(path, "r");
  if (file) {
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File not found: " + path);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("Client [%u] disconnected\n", num);
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("Client [%u] connected from %d.%d.%d.%d\n", 
                     num, ip[0], ip[1], ip[2], ip[3]);
        // Send initial data to newly connected client
        sendTelemetryData();
      }
      break;
      
    case WStype_TEXT:
      {
        String message = String((char*)payload);
        Serial.printf("Received: %s\n", message.c_str());
        
        // Handle commands from the dashboard
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, message);
        
        if (doc.containsKey("command")) {
          String command = doc["command"];
          
          if (command == "emergency_stop") {
            robotData.status = "Emergency Stop";
            robotData.speed = 0.0;
            Serial.println("Emergency stop activated!");
          } else if (command == "restart") {
            robotData.status = "Restarting";
            robotData.speed = 0.0;
            robotData.distance = 0.0;
            Serial.println("Robot restart initiated!");
          } else if (command == "calibrate") {
            robotData.status = "Calibrating";
            Serial.println("Calibration started!");
          }
        }
      }
      break;
  }
}

void updateRobotData() {
  // Simulate realistic robot data changes
  robotData.speed += (random(-20, 21) / 100.0); // -0.2 to +0.2 m/s change
  robotData.speed = constrain(robotData.speed, 0.0, 5.0);
  
  robotData.distance += robotData.speed * 0.1; // Distance increases with speed
  
  robotData.angle += random(-20, 21) / 10.0; // -2 to +2 degree change
  robotData.angle = fmod(robotData.angle, 360.0);
  
  robotData.temperature += (random(-50, 51) / 100.0); // -0.5 to +0.5°C change
  robotData.temperature = constrain(robotData.temperature, 20.0, 80.0);
  
  robotData.battery += (random(-10, 11) / 100.0); // -0.1 to +0.1V change
  robotData.battery = constrain(robotData.battery, 10.0, 14.0);
  
  robotData.propeller_rpm += random(-50, 51); // -50 to +50 RPM change
  robotData.propeller_rpm = constrain(robotData.propeller_rpm, 1000, 2000);
  
  robotData.propeller_thrust += random(-5, 6); // -5 to +5% change
  robotData.propeller_thrust = constrain(robotData.propeller_thrust, 70, 100);
  
  robotData.timestamp = millis();
  
  // Update status based on conditions
  if (robotData.temperature > 70) {
    robotData.status = "High Temp";
  } else if (robotData.battery < 11.0) {
    robotData.status = "Low Battery";
  } else if (robotData.speed > 0) {
    robotData.status = "Active";
  } else {
    robotData.status = "Idle";
  }
}

void sendTelemetryData() {
  // Create JSON payload with robot data
  DynamicJsonDocument doc(1024);
  doc["speed"] = robotData.speed;
  doc["distance"] = robotData.distance;
  doc["angle"] = robotData.angle;
  doc["temperature"] = robotData.temperature;
  doc["battery"] = robotData.battery;
  doc["propeller_rpm"] = robotData.propeller_rpm;
  doc["propeller_thrust"] = robotData.propeller_thrust;
  doc["status"] = robotData.status;
  doc["timestamp"] = robotData.timestamp;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Send to all connected WebSocket clients
  webSocket.broadcastTXT(jsonString);
}
