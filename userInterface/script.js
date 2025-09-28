// Honeycomb FRC Dashboard JavaScript
class HoneycombDashboard {
    
    //Constructor of elements for the dashboard
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
            speed: { 
                labels: [], 
                datasets: [] 
            },
            temperature: { 
                labels: [], 
                datasets: [] 
            },
            battery: { 
                labels: [], 
                datasets: [] 
            },
            dataComparison: { 
                labels: [], 
                measuredData: [], 
                expectedData: [] 
            }
        };
        this.maxDataPoints = 50;
        this.esp32Handler = null;
        this.esp32Connected = false;
        this.realTimeData = null; // Store real-time data from ESP32
        this.websocket = null;
        this.init();
    }

    //Initializing the dashboard
    init() {
        this.loadSettings();
        this.setupEventListeners();
        this.initializeCharts();
        this.initializeDataComparisonChart();
        this.startDataSimulation();
        this.updateClock();
        this.setupWidgetControls();
        this.setupChartControls();
        this.startPropellerAnimation();
        this.connectToESP32();
    }

    //Loading the settings from the localStorage
    loadSettings() {
        const esp32IP = localStorage.getItem('esp32IP') || '192.168.1.100';
        const esp32Port = parseInt(localStorage.getItem('esp32Port')) || 8080;
        const maxDataPoints = parseInt(localStorage.getItem('maxDataPoints')) || 50;
        
        this.esp32IP = esp32IP;
        this.esp32Port = esp32Port;
        this.maxDataPoints = maxDataPoints;
    }

    //Setting up the event listeners for the dashboard
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

    //Initializing the charts for the dashboard
    initializeCharts() {
        
        // Chart.js configuration with honeycomb theme
        const chartConfig = {
            responsive: true, //Make the charts responsive
            maintainAspectRatio: false, //Don't maintain the aspect ratio of height x width of the charts
            plugins: {
                legend: { //Legend for the charts
                    labels: {
                        color: '#2F1B14',
                        font: {
                            weight: 'bold'
                        }
                    }
                }
            },
            scales: {
                x: { //X-axis for the charts
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
                y: { //Y-axis for the charts
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
                ...chartConfig, //spread operator to copy the chartConfig
                plugins: {
                    ...chartConfig.plugins, //spread operator to copy the chartConfig.plugins
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
                ...chartConfig, //spread operator to copy the chartConfig
                plugins: {
                    ...chartConfig.plugins, //spread operator to copy the chartConfig.plugins
                    title: {
                        display: false
                    }
                }
            }
        });
    }

    initializeDataComparisonChart() {
        // Data Comparison Chart for measured vs expected values
        this.charts.dataComparison = new Chart(document.getElementById('dataComparisonChart'), {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Measured Values',
                    data: [],
                    borderColor: '#FFD700',
                    backgroundColor: 'rgba(255, 215, 0, 0.2)',
                    borderWidth: 3,
                    fill: false,
                    tension: 0.4,
                    pointBackgroundColor: '#FFD700',
                    pointBorderColor: '#FFA500',
                    pointBorderWidth: 2,
                    pointRadius: 4
                }, {
                    label: 'Expected Values',
                    data: [],
                    borderColor: '#00d4ff',
                    backgroundColor: 'rgba(0, 212, 255, 0.2)',
                    borderWidth: 3,
                    fill: false,
                    tension: 0.4,
                    pointBackgroundColor: '#00d4ff',
                    pointBorderColor: '#0099cc',
                    pointBorderWidth: 2,
                    pointRadius: 4,
                    borderDash: [5, 5]
                }]
            },
            options: {
                responsive: true, //Make the charts responsive
                maintainAspectRatio: false, //Don't maintain the aspect ratio of height x width of the charts
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
                    x: { //X-axis for the charts
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
                    y: { //Y-axis for the charts
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
            }
        });

        // Initialize with some sample data
        this.generateSampleComparisonData();
    }

    generateSampleComparisonData() {
        const timeLabels = [];
        const measuredData = [];
        const expectedData = [];
        
        for (let i = 0; i < 20; i++) {
            const time = new Date(Date.now() - (20 - i) * 1000);
            timeLabels.push(time.toLocaleTimeString());
            
            // Generate random realistic measured values with some variance
            const baseValue = Math.sin(i * 0.3) * 50 + 100;
            measuredData.push(baseValue + (Math.random() - 0.5) * 10);
            expectedData.push(baseValue);
        }
        
        //List will always follow the pattern: measuredData, expectedData, measuredData, expectedData, ...
        this.charts.dataComparison.data.labels = timeLabels;
        this.charts.dataComparison.data.datasets[0].data = measuredData;
        this.charts.dataComparison.data.datasets[1].data = expectedData;
        this.charts.dataComparison.update();
    }

    //This will be where we update the data comparison chart
    updateDataComparisonChart() {
        const now = new Date();
        const timeLabel = now.toLocaleTimeString();
        
        // Use real robot data if available, otherwise generate simulated data
        let measuredValue, expectedValue;
        
        if (this.realTimeData && this.realTimeData.speed !== undefined) {
            // Use real ESP32 data
            measuredValue = this.realTimeData.speed * 10 + 50; // Scale for chart display
            expectedValue = measuredValue + (Math.random() - 0.5) * 5; // Slight variance for expected
        } else {
            // Generate simulated data
            const baseValue = Math.sin(Date.now() * 0.001) * 50 + 100;
            measuredValue = baseValue + (Math.random() - 0.5) * 15;
            expectedValue = baseValue;
        }
        
        // Add new data point
        //List will always follow the pattern: measuredData, expectedData, measuredData, expectedData, ...
        this.charts.dataComparison.data.labels.push(timeLabel);
        this.charts.dataComparison.data.datasets[0].data.push(measuredValue);
        this.charts.dataComparison.data.datasets[1].data.push(expectedValue);
        
        // Keep only the last maxDataPoints
        if (this.charts.dataComparison.data.labels.length > this.maxDataPoints) {
            this.charts.dataComparison.data.labels.shift();
            this.charts.dataComparison.data.datasets[0].data.shift();
            this.charts.dataComparison.data.datasets[1].data.shift();
        }
        
        // Update the chart
        this.charts.dataComparison.update('none');
    }

    //Replace with the actual propeellow design soon
    startPropellerAnimation() {
        // Update propeller values periodically
        setInterval(() => {
            this.updatePropellerValues();
        }, 2000 /*2 seconds*/);
    }

    
    updatePropellerValues() {
        // Use real ESP32 data if available, otherwise simulate
        let rpm, thrust, status;
        
        //If the realTimeData is not undefined, then use the real ESP32 data
        if (this.realTimeData && this.realTimeData.propeller_rpm !== undefined) {
            
            // Use real ESP32 data
            rpm = this.realTimeData.propeller_rpm;
            thrust = this.realTimeData.propeller_thrust;
            status = this.realTimeData.status;
        } else {
            
            // Simulate varying propeller data
            rpm = 1000 + Math.random() * 500;
            thrust = 70 + Math.random() * 30;
            const statuses = ['Active', 'Optimal', 'High Load', 'Low Load'];
            status = statuses[Math.floor(Math.random() * statuses.length)];
        }
        
        document.getElementById('propellerRPM').textContent = Math.round(rpm);
        document.getElementById('propellerThrust').textContent = Math.round(thrust) + '%';
        document.getElementById('propellerStatus').textContent = status;
        
        // Calculate animation duration for one full rotation based on RPM
        // Formula: 60 seconds / RPM = seconds per rotation
        const propellerSvg = document.querySelector('.propeller-svg');
        const animationDuration = Math.max(0.5, 3 - (rpm - 1000) / 1000);
        propellerSvg.style.animationDuration = animationDuration + 's';
        
        // Track rotation count and add visual feedback
        this.updatePropellerRotation(rpm, animationDuration);
    }

    updatePropellerRotation(rpm, duration) {
        // Track rotation count
        if (!this.propellerRotationCount) {
            this.propellerRotationCount = 0;
        }
        
        // Increment rotation count
        this.propellerRotationCount++;
        
        // Log every 10 rotations for debugging
        if (this.propellerRotationCount % 10 === 0) {
            console.log(`Propeller completed ${this.propellerRotationCount} rotations at ${rpm} RPM`);
        }
        
        // Add visual feedback for completed rotations
        this.addRotationFeedback();
    }

    addRotationFeedback() {
        // Add a subtle visual effect when rotation completes
        const propellerSvg = document.querySelector('.propeller-svg');
        propellerSvg.style.filter = 'drop-shadow(0 0 20px rgba(255, 213, 128, 0.8))';
        
        // Reset the glow effect after a short delay
        setTimeout(() => {
            propellerSvg.style.filter = 'drop-shadow(0 0 15px rgba(255, 213, 128, 0.6))';
        }, 100);
    }

    // Connect to ESP32 WebSocket
    connectToESP32() {
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.hostname}:81`;
        
        console.log('Attempting to connect to ESP32 WebSocket:', wsUrl);
        
        this.websocket = new WebSocket(wsUrl);
        
        //Attempting to connect to the ESP32 WebSocket
        //If the ESP32 WebSocket is connected, then update the ESP32 button status to true
        this.websocket.onopen = (event) => {
            console.log('Connected to ESP32 WebSocket');
            this.esp32Connected = true;
            this.updateESP32ButtonStatus(true);
            this.addLogEntry('Connected to ESP32', 'success');
        };
        
        //If the ESP32 WebSocket is connected, then update the realTimeData
        this.websocket.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                this.realTimeData = data;
                this.processRealTimeData(data);
            } catch (error) {
                console.error('Error parsing ESP32 data:', error);
            }
        };
        
        //If the ESP32 WebSocket is disconnected, then update the ESP32 button status to false
        this.websocket.onclose = (event) => {
            console.log('ESP32 WebSocket disconnected');
            this.esp32Connected = false;
            this.realTimeData = null;
            this.updateESP32ButtonStatus(false);
            this.addLogEntry('Disconnected from ESP32', 'warning');
            
            // Attempt to reconnect after 3 seconds
            setTimeout(() => {
                if (!this.esp32Connected) {
                    this.connectToESP32();
                }
            }, 3000 /*  */);
        };
        
        //If the ESP32 WebSocket is error, then log the error
        this.websocket.onerror = (error) => {
            console.error('ESP32 WebSocket error:', error);
            this.addLogEntry('ESP32 connection error', 'error');
        };
    }

    // Process real-time data from ESP32
    processRealTimeData(data) {
        // Update robot data with ESP32 values
        this.robotData.speed = data.speed || this.robotData.speed;
        this.robotData.distance = data.distance || this.robotData.distance;
        this.robotData.angle = data.angle || this.robotData.angle;
        this.robotData.temperature = data.temperature || this.robotData.temperature;
        this.robotData.battery = data.battery || this.robotData.battery;
        
        // Update displays
        this.updateTelemetryDisplay();
        this.updateBatteryDisplay();
        this.updateCharts();
    }

    // Send command to ESP32
    sendESP32Command(command) {
        if (this.websocket && this.websocket.readyState === WebSocket.OPEN) {
            const message = JSON.stringify({ command: command });
            this.websocket.send(message);
            console.log('Sent command to ESP32:', command);
        } else {
            console.warn('ESP32 not connected, cannot send command');
        }
    }

    //Setting up the chart controls for the dashboard
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

            if (pauseBtn) { //If the pause button is found, then add an event listener to it
                pauseBtn.addEventListener('click', () => {
                    this.toggleChartPause(chartName, pauseBtn);
                });
            }

            if (resetBtn) { //If the reset button is found, then add an event listener to it
                resetBtn.addEventListener('click', () => {
                    this.resetChart(chartName);
                });
            }
        });
    }

    //Setting up the widget controls for the dashboard
    setupWidgetControls() {
        
        // Chart fullscreen
        const chartFullscreenBtn = document.querySelector('.chart-widget .widget-btn[title="Fullscreen"]');
        if (chartFullscreenBtn) { //If the chart fullscreen button is found, then add an event listener to it
            chartFullscreenBtn.addEventListener('click', () => {
                this.toggleChartFullscreen();
            });
        }

        // Propeller refresh
        const propellerRefreshBtn = document.querySelector('.propeller-widget .widget-btn[title="Refresh"]');
        if (propellerRefreshBtn) { //If the propeller refresh button is found, then add an event listener to it
            propellerRefreshBtn.addEventListener('click', () => {
                this.refreshPropeller();
            });
        }
    }

    //Updating the charts for the dashboard
    updateCharts() {

        const now = new Date(); //Instantiating and declearing a time in date format
        const timeLabel = now.toLocaleTimeString(); //Getting the current time in string format

        // Update Speed Chart
        this.updateChartData('speed', timeLabel, this.robotData.speed);

        // Update Temperature Chart
        this.updateChartData('temperature', timeLabel, this.robotData.temperature);

        // Update Battery Chart
        this.updateChartData('battery', timeLabel, this.robotData.battery);
    }

    //Updating the chart data for the dashboard
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

    //Toggling the chart pause for the dashboard
    toggleChartPause(chartName, button) {
        const icon = button.querySelector('i');

        //If the icon contains the class fa-pause, then remove the class fa-pause and add the class fa-play
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

    //Resetting the chart for the dashboard
    resetChart(chartName) {
        const chart = this.charts[chartName];
        if (!chart) return;

        chart.data.labels = [];
        chart.data.datasets[0].data = [];
        chart.update();
    }

    //Starting the data simulation for the dashboard
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
                this.updateDataComparisonChart();
            }
        }, 1000); // Update every second for better chart visualization
    }

    updateTelemetryDisplay() {
        // Update center telemetry (if it exists)
        const speedValue = document.getElementById('speedValue');
        const distanceValue = document.getElementById('distanceValue');
        const angleValue = document.getElementById('angleValue');
        const tempValue = document.getElementById('tempValue');
        
        if (speedValue) speedValue.textContent = `${this.robotData.speed.toFixed(1)} m/s`;
        if (distanceValue) distanceValue.textContent = `${this.robotData.distance.toFixed(1)} m`;
        if (angleValue) angleValue.textContent = `${Math.round(this.robotData.angle)}°`;
        if (tempValue) tempValue.textContent = `${Math.round(this.robotData.temperature)}°C`;
        
        // Update left panel telemetry
        document.getElementById('speedValueLeft').textContent = `${this.robotData.speed.toFixed(1)} m/s`;
        document.getElementById('distanceValueLeft').textContent = `${this.robotData.distance.toFixed(1)} m`;
        document.getElementById('angleValueLeft').textContent = `${Math.round(this.robotData.angle)}°`;
        document.getElementById('tempValueLeft').textContent = `${Math.round(this.robotData.temperature)}°C`;
        document.getElementById('batteryValueLeft').textContent = `${this.robotData.battery.toFixed(1)}V`;
        
        // Update status based on robot conditions
        let status = 'Active';
        if (this.robotData.temperature > 70) 
            {
            status = 'High Temp';
            }
        else if (this.robotData.battery < 11.0) 
            {
                status = 'Low Battery';
            }
        else if (!this.isConnected) 
            {
                status = 'Disconnected';
            }
        document.getElementById('statusValueLeft').textContent = status;
    }

    //Updating the battery display for the dashboard
    updateBatteryDisplay() {
        const batteryElement = document.querySelector('.battery-level span');
        batteryElement.textContent = `${this.robotData.battery.toFixed(1)}V`;
        
        // Update battery icon based on voltage
        const batteryIcon = document.querySelector('.battery-level i');
        if (this.robotData.battery > 12.0) 
        {
            batteryIcon.className = 'fas fa-battery-three-quarters';
        } 
        else if (this.robotData.battery > 11.0) 
        {
            batteryIcon.className = 'fas fa-battery-half';
        } 
        else if (this.robotData.battery > 10.0) 
        {
            batteryIcon.className = 'fas fa-battery-quarter';
        } 
        else 
        {
            batteryIcon.className = 'fas fa-battery-empty';
        }
    }

    //Updating the clock for the dashboard
    updateClock() {
        const updateTime = () => {
            const now = new Date(); //Instantiating and declaring a time in date format
            const timeString = now.toLocaleTimeString(); //Getting the current time in string format
            document.getElementById('currentTime').textContent = timeString;
        };
        
        updateTime();
        setInterval(updateTime, 1000);
    }

    //Sending the emergency stop command to the ESP32
    emergencyStop() {
        this.sendESP32Command('emergency_stop');
        this.addLogEntry('Emergency stop activated', 'error');
    }

    //Sending the restart command to the ESP32
    restartRobot() {
        this.sendESP32Command('restart');
        this.addLogEntry('Restarting robot...', 'warning');
    }

    //Sending the calibrate command to the ESP32
    calibrateRobot() {
        this.sendESP32Command('calibrate');
        this.addLogEntry('Starting calibration...', 'info');
    }

    //Updating the connection status for the dashboard
    updateConnectionStatus(connected) {
        const statusIndicator = document.querySelector('.status-indicator');
        const statusText = document.querySelector('.connection-status span');
        
        //If the connected is true, then update the connection status to connected
        if (connected) {
            statusIndicator.className = 'fas fa-circle status-indicator connected';
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.className = 'fas fa-circle status-indicator disconnected';
            statusText.textContent = 'Disconnected';
        }
    }

    //Adding the log entry to the dashboard
    addLogEntry(message, type = 'info') {
        const logContainer = document.querySelector('.event-log');
        const logEntry = document.createElement('div');
        logEntry.className = `log-entry ${type}`; //Adding the class log-entry and the type to the log entry
        
        const now = new Date();
        const timeString = now.toLocaleTimeString();
        
        //Log entry will always follow the pattern: time, message
        logEntry.innerHTML = `
            <span class="log-time">${timeString}</span>
            <span class="log-message">${message}</span>
        `;
        
        //Adding the log entry to the log container
        logContainer.insertBefore(logEntry, logContainer.firstChild);
        
        // Keep only last 10 entries
        //If the log container has more than 10 entries, then remove the last entry
        while (logContainer.children.length > 10) {
            logContainer.removeChild(logContainer.lastChild);
        }
    }

    //Toggling the chart fullscreen for the dashboard
    toggleChartFullscreen() {
        const chartWidget = document.querySelector('.chart-widget');
        
        //If the document fullscreen element is not found, then request the fullscreen for the chart widget
        if (!document.fullscreenElement) {
            chartWidget.requestFullscreen().catch(err => {
                console.log('Error attempting to enable fullscreen:', err);
            });
            //If the error is thrown, then log the error
        } else {
            document.exitFullscreen();
        }
    }

    //Refreshing the propeller values for the dashboard
    refreshPropeller() {
        this.updatePropellerValues();
        this.addLogEntry('Propeller data refreshed', 'info');
    }

    //Refreshing the telemetry values for the dashboard
    refreshTelemetry() {
        this.addLogEntry('Telemetry data refreshed', 'info');
        this.updateTelemetryDisplay();
    }

    //Opening the settings modal for the dashboard
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
        
        //If the modal is clicked, then remove the modal and the style
        modal.addEventListener('click', (e) => {
            if (e.target === modal) { //If the target is the modal, then remove the modal and the style
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
            
            //Adding the log entry to the dashboard
            this.addLogEntry('Settings saved', 'success');
            document.body.removeChild(modal);
            document.head.removeChild(style);
        });

        // Cancel functionality
        modal.querySelector('.btn-secondary').addEventListener('click', () => {
            //Removing the modal and the style
            document.body.removeChild(modal);
            document.head.removeChild(style);
        });
    }

    //Toggling the ESP32 connection for the dashboard
    toggleESP32Connection() {
        const button = document.getElementById('esp32ConnectBtn');
        const icon = button.querySelector('i');
        const text = button.querySelector('span');

        //If the ESP32 is not connected, then connect to the ESP32
        if (!this.esp32Connected) {
            // Connect to ESP32
            this.esp32Handler = new ESP32DataHandler(this);
            this.esp32Handler.connect(this.esp32IP, this.esp32Port);
            
            //Updating the button appearance
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
            
            //Updating the ESP32 connection status
            this.esp32Connected = false;
            this.addLogEntry('Disconnected from ESP32', 'warning');
        }
    }

    //Updating the ESP32 button status for the dashboard
    updateESP32ButtonStatus(connected) {
        const button = document.getElementById('esp32ConnectBtn');
        const icon = button.querySelector('i');
        const text = button.querySelector('span');

        if (connected) 
        { //If the ESP32 is connected, then update the ESP32 button status to connected
            button.classList.add('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'ESP32 Connected';
        } 
        else 
        {
            button.classList.remove('connected');
            icon.className = 'fas fa-wifi';
            text.textContent = 'Connect ESP32';
        }
    }

    //Handling the widget control for the dashboard
    handleWidgetControl(button) {
        const title = button.getAttribute('title');
        const widget = button.closest('.widget');
        
        switch (title) {
            //If the title is Fullscreen, then toggle the chart fullscreen
            case 'Fullscreen':
                if (widget.classList.contains('chart-widget')) {
                    this.toggleChartFullscreen();
                } 
                else 
                {
                    this.toggleCameraFullscreen();
                }
                break;
            
            //If the title is Settings, then open the settings modal
            case 'Settings':
                this.openSettings();
                break;
            
            //If the title is Refresh, then refresh the propeller or telemetry
            case 'Refresh':
                if (widget.classList.contains('propeller-widget')) {
                    this.refreshPropeller();
                } else {
                    this.refreshTelemetry();
                }
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

    //Connecting to the ESP32
    connect(esp32IP = '192.168.1.100', port = 8080) {
        try {
            this.websocket = new WebSocket(`ws://${esp32IP}:${port}/telemetry`);
            
            //If the ESP32 WebSocket is opened, then add the log entry to the dashboard
            this.websocket.onopen = () => {
                console.log('Connected to ESP32');
                this.dashboard.addLogEntry('ESP32 connected', 'success');
                this.dashboard.updateESP32ButtonStatus(true);
                this.reconnectAttempts = 0;
            };

            //If the ESP32 WebSocket is message, then process the ESP32 data
            this.websocket.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    this.processESP32Data(data);
                } catch (error) {
                    console.error('Error parsing ESP32 data:', error);
                }
            };

            //If the ESP32 WebSocket is closed, then add the log entry to the dashboard
            this.websocket.onclose = () => {
                console.log('ESP32 connection closed');
                this.dashboard.addLogEntry('ESP32 disconnected', 'warning');
                this.dashboard.updateESP32ButtonStatus(false);
                this.attemptReconnect();
            };

            //If the ESP32 WebSocket is error, then add the log entry to the dashboard
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
        // Update robot data with ESP32 telemetry if speed, temperature, battery, distance, and angle are not undefined
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

    //Attempting to reconnect to the ESP32
    attemptReconnect() {
        if (this.reconnectAttempts < this.maxReconnectAttempts) { //If the reconnect attempts are less than the max reconnect attempts, then reconnect to the ESP32
            this.reconnectAttempts++;
            setTimeout(() => {
                console.log(`Attempting to reconnect to ESP32 (${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
                this.connect();
            }, 2000 * this.reconnectAttempts);
        }
    }

    //Disconnecting from the ESP32
    disconnect() {
        //If the websocket is not null, then close the websocket
        if (this.websocket) {
            this.websocket.close();
            this.websocket = null;
        }
    }

    //Sending the command to the ESP32
    sendCommand(command) {
        //If the websocket is not null and the websocket is open, then send the command to the ESP32
        if (this.websocket && this.websocket.readyState === WebSocket.OPEN) {
            this.websocket.send(JSON.stringify(command));
        } else {
            //If the websocket is null or the websocket is not open, then log the error
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

//Generating the random data for the dashboard
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