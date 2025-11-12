# ESP32 Setup Guide - Honeycomb Devils Invent 2025

### 1. Install Required Libraries
In Arduino IDE, install these libraries via Library Manager:
- **ArduinoJson** by Benoit Blanchon (v6.x)
- **WebSockets** by Markus Sattler
- **SPIFFS** (included with ESP32 core)

### 2. Hardware Setup
- **ESP32 Dev Board** (NodeMCU-32S or similar)
- **USB Cable** for programming and power
- **LED** (optional, uses built-in LED on GPIO2)

### 3. Arduino IDE Configuration
1. Install ESP32 board package:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

2. Select your board:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

3. Configure settings:
   - **Upload Speed**: 115200
   - **CPU Frequency**: 240MHz
   - **Flash Frequency**: 80MHz
   - **Flash Mode**: QIO
   - **Flash Size**: 4MB (32Mb)
   - **Partition Scheme**: Default 4MB with spiffs
   - **Core Debug Level**: None

### 4. Upload Files to ESP32

#### Step 4a: Upload the Arduino Sketch
1. Open `esp32_server.ino` in Arduino IDE
2. **IMPORTANT**: Change Wi-Fi credentials in the code:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";        // Change this!
   const char* password = "YOUR_WIFI_PASSWORD"; // Change this!
   ```
3. Connect ESP32 via USB
4. Select correct COM port (Tools → Port)
5. Click Upload (Ctrl+U)

#### Step 4b: Upload Web Files to SPIFFS
1. Install ESP32 Filesystem Uploader:
   - Tools → Board → ESP32 Dev Module
   - Tools → Partition Scheme → Default 4MB with spiffs
   - Tools → ESP32 Sketch Data Upload → Upload

2. **OR** use ESP32FS plugin:
   - Download: https://github.com/me-no-dev/arduino-esp32fs-plugin
   - Install in Arduino/libraries/tools/
   - Tools → ESP32 Sketch Data Upload

### 5. File Structure on ESP32
```
SPIFFS Root:
├── index.html
├── style.css
├── script.js
└── esp32-sample-data.json (optional)
```

## 🔧 Usage

### 1. Power On ESP32
1. Connect ESP32 to power (USB or external 5V)
2. Watch Serial Monitor (115200 baud) for:
   ```
   Starting Honeycomb Devils Invent 2025 Dashboard...
   Wi-Fi connected!
   IP address: 192.168.1.XXX
   HTTP server started on port 80
   WebSocket server started on port 81
   Dashboard available at: http://192.168.1.XXX
   ```

### 2. Connect to Dashboard
1. **On same Wi-Fi network**: Open browser to `http://192.168.1.XXX`
2. **Direct connection**: Connect to ESP32's Wi-Fi hotspot (if configured)
3. **Mobile devices**: Works on phones/tablets too!

### 3. Features Available
- ✅ **Real-time telemetry** via WebSocket
- ✅ **Interactive controls** (Emergency Stop, Restart, Calibrate)
- ✅ **Live charts** with ESP32 data
- ✅ **Animated propeller** with real RPM data
- ✅ **Mobile responsive** design

## 🔍 Troubleshooting

### Common Issues

#### "SPIFFS Mount Failed"
- **Solution**: Use Tools → ESP32 Sketch Data Upload
- **Check**: Partition scheme is "Default 4MB with spiffs"

#### "Wi-Fi Connection Failed"
- **Check**: Wi-Fi credentials in code
- **Check**: Network allows new devices
- **Check**: Signal strength (move closer to router)

#### "WebSocket Connection Failed"
- **Check**: Firewall settings
- **Check**: ESP32 IP address in Serial Monitor
- **Check**: Port 81 is not blocked

#### "Files Not Loading"
- **Solution**: Re-upload files to SPIFFS
- **Check**: File names match exactly (case-sensitive)
- **Check**: File sizes (ESP32 has limited storage)

### Serial Monitor Output
Watch for these messages:
```
✅ "Wi-Fi connected!" - Good connection
✅ "HTTP server started" - Web server running
✅ "WebSocket server started" - Real-time data ready
✅ "Client connected" - Dashboard connected
❌ "Connection failed" - Check credentials
❌ "SPIFFS Mount Failed" - Re-upload files
```

## 📱 Mobile Usage

### Android/iOS Setup
1. Connect phone to same Wi-Fi as ESP32
2. Open browser (Chrome, Safari, etc.)
3. Navigate to ESP32 IP address
4. **Add to Home Screen** for app-like experience

### Performance Tips
- **Close other apps** for better performance
- **Keep ESP32 close** to router for stable connection
- **Use landscape mode** for better dashboard view

## 🔄 Updates & Maintenance

### Updating Dashboard Files
1. Modify HTML/CSS/JS files
2. Re-upload to SPIFFS using ESP32 Sketch Data Upload
3. ESP32 automatically serves new files

### Updating Arduino Code
1. Modify `esp32_server.ino`
2. Upload normally (Ctrl+U)
3. ESP32 restarts with new code

### Adding Sensors
1. Wire sensors to ESP32 GPIO pins
2. Add sensor reading code to `updateRobotData()` function
3. Include sensor data in JSON payload
4. Update dashboard JavaScript to display new data

## 🎯 Next Steps

1. **Add Real Sensors**:
   - Temperature sensor (DS18B20)
   - IMU/Gyroscope (MPU6050)
   - Battery voltage divider
   - Motor encoders

2. **Enhance Features**:
   - Data logging to SD card
   - OTA (Over-The-Air) updates
   - Multiple robot support
   - Video streaming

3. **Production Deployment**:
   - Custom PCB design
   - Weatherproof enclosure
   - Battery power management
   - Fail-safe mechanisms

- **Serial Monitor**: Always check for error messages
- **Network Tools**: Use `ping 192.168.1.XXX` to test connectivity
- **Browser DevTools**: F12 → Console for JavaScript errors
- **ESP32 Documentation**: https://docs.espressif.com/
