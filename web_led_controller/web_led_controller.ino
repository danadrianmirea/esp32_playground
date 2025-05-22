#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// Built-in LED pin
const int ledPin = 2;  // Most ESP32 boards have the built-in LED on GPIO 2

// Create WebServer object
WebServer server(80);

// LED state
bool ledState = false;

// HTML content
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 LED Controller</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 50px;
        }
        .button {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 4px;
        }
        .button:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <h1>ESP32 LED Controller</h1>
    <p>Click the button to toggle the LED:</p>
    <button class="button" onclick="toggleLED()">Toggle LED</button>
    <script>
        function toggleLED() {
            fetch('/toggle')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                });
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    
    // Initialize LED pin
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Set up server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle", HTTP_GET, handleToggle);
    
    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    server.send(200, "text/html", htmlContent);
}

void handleToggle() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    server.send(200, "text/plain", ledState ? "LED ON" : "LED OFF");
} 