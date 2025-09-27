// Honeycomb FRC Dashboard JavaScript
class HoneycombDashboard {
    constructor() {
        this.isConnected = true;
        this.robotData = {
            speed: 0,
            distance: 0,
            angle: 0,
            temperature: 25,
            battery: 12.4
        };
        this.charts = {};
        this.chartData = {
            speed: { labels: [], datasets: [] },
            temperature: { labels: [], datasets: [] },
            battery: { labels: [], datasets: [] }
        };
        this.maxDataPoints = 50;
        this.esp32Handler = null;
        this.esp32Connected = false;
        this.init();
    }

    init() {
        this.loadSettings();
        this.setupEventListeners();
        this.initializeCharts();
        this.startDataSimulation();
        this.updateClock();
        this.setupWidgetControls();
        this.setupChartControls();
    }

    loadSettings() {
        // Load settings from localStorage
        const esp32IP = localStorage.getItem('esp32IP') || '192.168.1.100';
        const esp32Port = parseInt(localStorage.getItem('esp32Port')) || 8080;
        const maxDataPoints = parseInt(localStorage.getItem('maxDataPoints')) || 50;
        
        this.esp32IP = esp32IP;
        this.esp32Port = esp32Port;
        this.maxDataPoints = maxDataPoints;
    }

    setupEventListeners() {
        // Emergency stop button
        document.querySelector('.action-btn.emergency').addEventListener('click', () => {
            this.emergencyStop();
        });

        // Restart button
        document.querySelector('.action-btn.restart').addEventListener('click', () => {
            this.restartRobot();
        });

        // Calibrate button
        document.querySelector('.action-btn.calibrate').addEventListener('click', () => {
            this.calibrateRobot();
        });

        // ESP32 connection button
        document.getElementById('esp32ConnectBtn').addEventListener('click', () => {
            this.toggleESP32Connection();
        });

        // Settings button
        document.querySelector('.settings-btn').addEventListener('click', () => {
            this.openSettings();
        });

        // Widget controls
        document.querySelectorAll('.widget-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                this.handleWidgetControl(e.target.closest('.widget-btn'));
            });
        });
    }

    initializeCharts() {
        // Chart.js configuration with honeycomb theme
        const chartConfig = {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    labels: {
                        color: '#2F1B14',
                        font: {
                            weight: 'bold'
                        }
                    }
                }
            },
            scales: {
                x: {
                    ticks: {
                        color: '#8B4513',
                        font: {
                            weight: 'bold'
                        }
                    },
                    grid: {
                        color: 'rgba(255, 140, 0, 0.2)'
                    }
                },
                y: {
                    ticks: {
                        color: '#8B4513',
                        font: {
                            weight: 'bold'
                        }
                    },
                    grid: {
                        color: 'rgba(255, 140, 0, 0.2)'
                    }
                }
            }
        };

        // Speed Chart
        this.charts.speed = new Chart(document.getElementById('speedChart'), {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Speed (m/s)',
                    data: [],
                    borderColor: '#FFD700',
                    backgroundColor: 'rgba(255, 215, 0, 0.2)',
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointBackgroundColor: '#FFA500',
                    pointBorderColor: '#FFD700',
                    pointBorderWidth: 2,
                    pointRadius: 4
                }]
            },
            options: {
                ...chartConfig,
                plugins: {
                    ...chartConfig.plugins,
                    title: {
                        display: false
                    }
                }
            }
        });

        // Temperature Chart
        this.charts.temperature = new Chart(document.getElementById('temperatureChart'), {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Temperature (°C)',
                    data: [],
                    borderColor: '#FF8C00',
                    backgroundColor: 'rgba(255, 140, 0, 0.2)',
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointBackgroundColor: '#FF4500',
                    pointBorderColor: '#FF8C00',
                    pointBorderWidth: 2,
                    pointRadius: 4
                }]
            },
            options: {
                ...chartConfig,
                plugins: {
                    ...chartConfig.plugins,
                    title: {
                        display: false
                    }
                }
            }
        });

        // Battery Chart
        this.charts.battery = new Chart(document.getElementById('batteryChart'), {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Battery (V)',
                    data: [],
                    borderColor: '#32CD32',
                    backgroundColor: 'rgba(50, 205, 50, 0.2)',
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointBackgroundColor: '#228B22',
                    pointBorderColor: '#32CD32',
                    pointBorderWidth: 2,
                    pointRadius: 4
                }]
            },
            options: {
                ...chartConfig,
                plugins: {
                    ...chartConfig.plugins,
                    title: {
                        display: false
                    }
                }
            }
        });
    }

    setupChartControls() {
        // Chart control buttons
        const chartControls = {
            speed: { pause: 'speedPauseBtn', reset: 'speedResetBtn' },
            temperature: { pause: 'tempPauseBtn', reset: 'tempResetBtn' },
            battery: { pause: 'batteryPauseBtn', reset: 'batteryResetBtn' }
        };

        Object.keys(chartControls).forEach(chartName => {
            const pauseBtn = document.getElementById(chartControls[chartName].pause);
            const resetBtn = document.getElementById(chartControls[chartName].reset);

            if (pauseBtn) {
                pauseBtn.addEventListener('click', () => {
                    this.toggleChartPause(chartName, pauseBtn);
                });
            }

            if (resetBtn) {
                resetBtn.addEventListener('click', () => {
                    this.resetChart(chartName);
                });
            }
        });
    }

    setupWidgetControls() {
        // Camera fullscreen
        document.querySelector('.camera-widget .widget-btn[title="Fullscreen"]').addEventListener('click', () => {
            this.toggleCameraFullscreen();
        });

        // Telemetry refresh
        document.querySelector('.telemetry-widget .widget-btn[title="Refresh"]').addEventListener('click', () => {
            this.refreshTelemetry();
        });
    }

    updateCharts() {
        const now = new Date();
        const timeLabel = now.toLocaleTimeString();

        // Update Speed Chart
        this.updateChartData('speed', timeLabel, this.robotData.speed);

        // Update Temperature Chart
        this.updateChartData('temperature', timeLabel, this.robotData.temperature);

        // Update Battery Chart
        this.updateChartData('battery', timeLabel, this.robotData.battery);
    }

    updateChartData(chartName, label, value) {
        const chart = this.charts[chartName];
        if (!chart) return;

        // Add new data point
        chart.data.labels.push(label);
        chart.data.datasets[0].data.push(value);

        // Keep only the last maxDataPoints
        if (chart.data.labels.length > this.maxDataPoints) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }

        // Update the chart
        chart.update('none');
    }

    toggleChartPause(chartName, button) {
        const icon = button.querySelector('i');
        if (icon.classList.contains('fa-pause')) {
            icon.classList.remove('fa-pause');
            icon.classList.add('fa-play');
            this.charts[chartName].options.animation = false;
        } else {
            icon.classList.remove('fa-play');
            icon.classList.add('fa-pause');
            this.charts[chartName].options.animation = true;
        }
    }

    resetChart(chartName) {
        const chart = this.charts[chartName];
        if (!chart) return;

        chart.data.labels = [];
        chart.data.datasets[0].data = [];
        chart.update();
    }

    startDataSimulation() {
        setInterval(() => {
            if (this.isConnected) {
                // Simulate ESP32 robot data changes
                this.robotData.speed += (Math.random() - 0.5) * 0.2;
                this.robotData.speed = Math.max(0, Math.min(5, this.robotData.speed));
                
                this.robotData.distance += this.robotData.speed * 0.1;
                
                this.robotData.angle += (Math.random() - 0.5) * 2;
                this.robotData.angle = this.robotData.angle % 360;
                
                this.robotData.temperature += (Math.random() - 0.5) * 0.5;
                this.robotData.temperature = Math.max(20, Math.min(80, this.robotData.temperature));
                
                this.robotData.battery += (Math.random() - 0.5) * 0.1;
                this.robotData.battery = Math.max(10.0, Math.min(14.0, this.robotData.battery));
                
                this.updateTelemetryDisplay();
                this.updateBatteryDisplay();
                this.updateCharts();
            }
        }, 1000); // Update every second for better chart visualization
    }

    updateTelemetryDisplay() {
        document.getElementById('speedValue').textContent = `${this.robotData.speed.toFixed(1)} m/s`;
        document.getElementById('distanceValue').textContent = `${this.robotData.distance.toFixed(1)} m`;
        document.getElementById('angleValue').textContent = `${Math.round(this.robotData.angle)}°`;
        document.getElementById('tempValue').textContent = `${Math.round(this.robotData.temperature)}°C`;
    }

    updateBatteryDisplay() {
        const batteryElement = document.querySelector('.battery-level span');
        batteryElement.textContent = `${this.robotData.battery.toFixed(1)}V`;
        
        // Update battery icon based on voltage
        const batteryIcon = document.querySelector('.battery-level i');
        if (this.robotData.battery > 12.0) {
            batteryIcon.className = 'fas fa-battery-three-quarters';
        } else if (this.robotData.battery > 11.0) {
            batteryIcon.className = 'fas fa-battery-half';
        } else if (this.robotData.battery > 10.0) {
            batteryIcon.className = 'fas fa-battery-quarter';
        } else {
            batteryIcon.className = 'fas fa-battery-empty';
        }
    }

    updateClock() {
        const updateTime = () => {
            const now = new Date();
            const timeString = now.toLocaleTimeString();
            document.getElementById('currentTime').textContent = timeString;
        };
        
        updateTime();
        setInterval(updateTime, 1000);
    }

    emergencyStop() {
        this.isConnected = false;
        this.updateConnectionStatus(false);
        
        this.addLogEntry('Emergency stop activated', 'error');
        
        // Reconnect after 3 seconds
        setTimeout(() => {
            this.isConnected = true;
            this.updateConnectionStatus(true);
            this.addLogEntry('System reconnected', 'success');
        }, 3000);
    }

    restartRobot() {
        this.addLogEntry('Restarting robot...', 'warning');
        
        setTimeout(() => {
            this.robotData = {
                speed: 0,
                distance: 0,
                angle: 0,
                temperature: 25,
                battery: 12.4
            };
            this.updateTelemetryDisplay();
            this.addLogEntry('Robot restarted successfully', 'success');
        }, 2000);
    }

    calibrateRobot() {
        this.addLogEntry('Starting calibration...', 'info');
        
        // Simulate calibration process
        let progress = 0;
        const calibrationInterval = setInterval(() => {
            progress += 10;
            this.addLogEntry(`Calibration progress: ${progress}%`, 'info');
            
            if (progress >= 100) {
                clearInterval(calibrationInterval);
                this.addLogEntry('Calibration completed', 'success');
            }
        }, 500);
    }

    updateConnectionStatus(connected) {
        const statusIndicator = document.querySelector('.status-indicator');
        const statusText = document.querySelector('.connection-status span');
        
        if (connected) {
            statusIndicator.className = 'fas fa-circle status-indicator connected';
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.className = 'fas fa-circle status-indicator disconnected';
            statusText.textContent = 'Disconnected';
        }
    }

    addLogEntry(message, type = 'info') {
        const logContainer = document.querySelector('.event-log');
        const logEntry = document.createElement('div');
        logEntry.className = `log-entry ${type}`;
        
        const now = new Date();
        const timeString = now.toLocaleTimeString();
        
        logEntry.innerHTML = `
            <span class="log-time">${timeString}</span>
            <span class="log-message">${message}</span>
        `;
        
        logContainer.insertBefore(logEntry, logContainer.firstChild);
        
        // Keep only last 10 entries
        while (logContainer.children.length > 10) {
            logContainer.removeChild(logContainer.lastChild);
        }
    }

    toggleCameraFullscreen() {
        const cameraWidget = document.querySelector('.camera-widget');
        
        if (!document.fullscreenElement) {
            cameraWidget.requestFullscreen().catch(err => {
                console.log('Error attempting to enable fullscreen:', err);
            });
        } else {
            document.exitFullscreen();
        }
    }

    refreshTelemetry() {
        this.addLogEntry('Telemetry data refreshed', 'info');
        this.updateTelemetryDisplay();
    }

    openSettings() {
        // Create a simple settings modal
        const modal = document.createElement('div');
        modal.className = 'settings-modal';
        modal.innerHTML = `
            <div class="modal-content">
                <div class="modal-header">
                    <h3>Dashboard Settings</h3>
                    <button class="close-btn">&times;</button>
                </div>
                <div class="modal-body">
                    <div class="setting-group">
                        <label>ESP32 IP Address</label>
                        <input type="text" id="esp32IP" value="192.168.1.100" placeholder="192.168.1.100">
                    </div>
                    <div class="setting-group">
                        <label>ESP32 Port</label>
                        <input type="number" id="esp32Port" value="8080" min="1000" max="65535">
                    </div>
                    <div class="setting-group">
                        <label>Update Frequency (ms)</label>
                        <input type="number" value="1000" min="100" max="5000">
                    </div>
                    <div class="setting-group">
                        <label>Auto-reconnect</label>
                        <input type="checkbox" checked>
                    </div>
                    <div class="setting-group">
                        <label>Chart Data Points</label>
                        <input type="number" id="maxDataPoints" value="50" min="10" max="200">
                    </div>
                </div>
                <div class="modal-footer">
                    <button class="btn-primary">Save</button>
                    <button class="btn-secondary">Cancel</button>
                </div>
            </div>
        `;
        
        // Add modal styles
        const style = document.createElement('style');
        style.textContent = `
            .settings-modal {
                position: fixed;
                top: 0;
                left: 0;
                width: 100%;
                height: 100%;
                background: rgba(0, 0, 0, 0.8);
                display: flex;
                align-items: center;
                justify-content: center;
                z-index: 1000;
            }
            .modal-content {
                background: rgba(255, 255, 255, 0.1);
                backdrop-filter: blur(20px);
                border-radius: 12px;
                padding: 2rem;
                min-width: 400px;
                border: 1px solid rgba(255, 255, 255, 0.2);
            }
            .modal-header {
                display: flex;
                justify-content: space-between;
                align-items: center;
                margin-bottom: 1.5rem;
                padding-bottom: 1rem;
                border-bottom: 1px solid rgba(255, 255, 255, 0.2);
            }
            .close-btn {
                background: none;
                border: none;
                color: #ffffff;
                font-size: 1.5rem;
                cursor: pointer;
            }
            .setting-group {
                margin-bottom: 1rem;
            }
            .setting-group label {
                display: block;
                margin-bottom: 0.5rem;
                color: #ffffff;
            }
            .setting-group input,
            .setting-group select {
                width: 100%;
                padding: 0.5rem;
                background: rgba(255, 255, 255, 0.1);
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 6px;
                color: #ffffff;
            }
            .modal-footer {
                display: flex;
                gap: 1rem;
                justify-content: flex-end;
                margin-top: 1.5rem;
                padding-top: 1rem;
                border-top: 1px solid rgba(255, 255, 255, 0.2);
            }
            .btn-primary,
            .btn-secondary {
                padding: 0.5rem 1rem;
                border: none;
                border-radius: 6px;
                cursor: pointer;
            }
            .btn-primary {
                background: #00d4ff;
                color: #ffffff;
            }
            .btn-secondary {
                background: rgba(255, 255, 255, 0.1);
                color: #ffffff;
                border: 1px solid rgba(255, 255, 255, 0.2);
            }
        `;
        
        document.head.appendChild(style);
        document.body.appendChild(modal);
        
        // Close modal functionality
        modal.querySelector('.close-btn').addEventListener('click', () => {
            document.body.removeChild(modal);
            document.head.removeChild(style);
        });
        
        modal.addEventListener('click', (e) => {
            if (e.target === modal) {
                document.body.removeChild(modal);
                document.head.removeChild(style);
            }
        });

        // Save settings functionality
        modal.querySelector('.btn-primary').addEventListener('click', () => {
            const esp32IP = document.getElementById('esp32IP').value;
            const esp32Port = parseInt(document.getElementById('esp32Port').value);
            const maxDataPoints = parseInt(document.getElementById('maxDataPoints').value);
            
            // Update dashboard settings
            this.maxDataPoints = maxDataPoints;
            
            // Save to localStorage
            localStorage.setItem('esp32IP', esp32IP);
            localStorage.setItem('esp32Port', esp32Port);
            localStorage.setItem('maxDataPoints', maxDataPoints);
            
            this.addLogEntry('Settings saved', 'success');
            document.body.removeChild(modal);
            document.head.removeChild(style);
        });

        // Cancel functionality
        modal.querySelector('.btn-secondary').addEventListener('click', () => {
            document.body.removeChild(modal);
            document.head.removeChild(style);
        });
    }

    toggleESP32Connection() {
        const button = document.getElementById('esp32ConnectBtn');
        const icon = button.querySelector('i');
        const text = button.querySelector('span');

        if (!this.esp32Connected) {
            // Connect to ESP32
            this.esp32Handler = new ESP32DataHandler(this);
            this.esp32Handler.connect(this.esp32IP, this.esp32Port);
            
            // Update button appearance
            button.classList.add('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'Connecting...';
            
            this.esp32Connected = true;
            this.addLogEntry('Attempting to connect to ESP32...', 'info');
        } else {
            // Disconnect from ESP32
            if (this.esp32Handler) {
                this.esp32Handler.disconnect();
                this.esp32Handler = null;
            }
            
            // Update button appearance
            button.classList.remove('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'Connect ESP32';
            
            this.esp32Connected = false;
            this.addLogEntry('Disconnected from ESP32', 'warning');
        }
    }

    updateESP32ButtonStatus(connected) {
        const button = document.getElementById('esp32ConnectBtn');
        const icon = button.querySelector('i');
        const text = button.querySelector('span');

        if (connected) {
            button.classList.add('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'ESP32 Connected';
        } else {
            button.classList.remove('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'Connect ESP32';
        }
    }

    handleWidgetControl(button) {
        const title = button.getAttribute('title');
        
        switch (title) {
            case 'Fullscreen':
                this.toggleCameraFullscreen();
                break;
            case 'Settings':
                this.openSettings();
                break;
            case 'Refresh':
                this.refreshTelemetry();
                break;
        }
    }
}

// Initialize dashboard when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    new HoneycombDashboard();
});

// ESP32 Data Integration Functions
class ESP32DataHandler {
    constructor(dashboard) {
        this.dashboard = dashboard;
        this.websocket = null;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 5;
    }

    connect(esp32IP = '192.168.1.100', port = 8080) {
        try {
            this.websocket = new WebSocket(`ws://${esp32IP}:${port}/telemetry`);
            
            this.websocket.onopen = () => {
                console.log('Connected to ESP32');
                this.dashboard.addLogEntry('ESP32 connected', 'success');
                this.dashboard.updateESP32ButtonStatus(true);
                this.reconnectAttempts = 0;
            };

            this.websocket.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    this.processESP32Data(data);
                } catch (error) {
                    console.error('Error parsing ESP32 data:', error);
                }
            };

            this.websocket.onclose = () => {
                console.log('ESP32 connection closed');
                this.dashboard.addLogEntry('ESP32 disconnected', 'warning');
                this.dashboard.updateESP32ButtonStatus(false);
                this.attemptReconnect();
            };

            this.websocket.onerror = (error) => {
                console.error('ESP32 WebSocket error:', error);
                this.dashboard.addLogEntry('ESP32 connection error', 'error');
            };
        } catch (error) {
            console.error('Failed to connect to ESP32:', error);
            this.dashboard.addLogEntry('Failed to connect to ESP32', 'error');
        }
    }

    processESP32Data(data) {
        // Update robot data with ESP32 telemetry
        if (data.speed !== undefined) {
            this.dashboard.robotData.speed = data.speed;
        }
        if (data.temperature !== undefined) {
            this.dashboard.robotData.temperature = data.temperature;
        }
        if (data.battery !== undefined) {
            this.dashboard.robotData.battery = data.battery;
        }
        if (data.distance !== undefined) {
            this.dashboard.robotData.distance = data.distance;
        }
        if (data.angle !== undefined) {
            this.dashboard.robotData.angle = data.angle;
        }

        // Update displays and charts
        this.dashboard.updateTelemetryDisplay();
        this.dashboard.updateBatteryDisplay();
        this.dashboard.updateCharts();
    }

    attemptReconnect() {
        if (this.reconnectAttempts < this.maxReconnectAttempts) {
            this.reconnectAttempts++;
            setTimeout(() => {
                console.log(`Attempting to reconnect to ESP32 (${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
                this.connect();
            }, 2000 * this.reconnectAttempts);
        }
    }

    disconnect() {
        if (this.websocket) {
            this.websocket.close();
            this.websocket = null;
        }
    }

    sendCommand(command) {
        if (this.websocket && this.websocket.readyState === WebSocket.OPEN) {
            this.websocket.send(JSON.stringify(command));
        } else {
            console.warn('ESP32 not connected, cannot send command');
        }
    }
}

// Add some additional utility functions
function formatTime(seconds) {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;
    return `${minutes}:${remainingSeconds.toString().padStart(2, '0')}`;
}

function generateRandomData() {
    return {
        speed: Math.random() * 5,
        distance: Math.random() * 100,
        angle: Math.random() * 360,
        temperature: 20 + Math.random() * 60
    };
}

// Export for potential module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { HoneycombDashboard, ESP32DataHandler };
}