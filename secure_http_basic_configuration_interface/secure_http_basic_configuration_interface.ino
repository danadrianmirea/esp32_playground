#include <WiFi.h>
#include "../libraries/ESP32_Shared_Config/ESP32_Shared_Config.h"

#define LOG_INTERVAL 1000
#define LED_PIN 2  // Built-in LED pin

// Create web server on port 80
WiFiServer server(80);

WiFiClient sseClient;
unsigned long lastSSEUpdate = 0;
bool sseActive = false;
bool ledState = false;

TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t otherTaskHandle = NULL;

void handleNewClient();
void updateSSEClient();
void webServerTask(void *pvParameters);
void otherTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();

  xTaskCreatePinnedToCore(
    webServerTask,      // Task function
    "WebServerTask",    // Name
    8192,               // Stack size
    NULL,               // Parameter
    1,                  // Priority
    &webServerTaskHandle, // Task handle
    0                   // Core 0
  );

  xTaskCreatePinnedToCore(
    otherTask,          // Task function
    "OtherTask",        // Name
    4096,               // Stack size
    NULL,               // Parameter
    1,                  // Priority
    &otherTaskHandle,   // Task handle
    1                   // Core 1
  );
}

void handleNewClient() {
  WiFiClient client = server.available();
  if (!client) return;

  String currentLine = "";
  String requestLine = "";
  String authHeader = "";
  bool isAuthenticated = false;
  unsigned long startTime = millis();

  // Wait up to 1000ms for the client to send data
  while (client.connected() && !client.available() && millis() - startTime < 1000) {
    delay(1);
  }

  while (client.connected() && client.available()) {
    char c = client.read();
    if (c == '\n') {
      if (currentLine.length() == 0) {
        // Check authentication
        if (authHeader.indexOf("Authorization: Basic ") >= 0) {
          String base64Credentials = authHeader.substring(authHeader.indexOf("Basic ") + 6);
          base64Credentials.trim();
          
          // Decode base64 credentials
          String credentials = base64Decode(base64Credentials);
          int colonIndex = credentials.indexOf(':');
          
          if (colonIndex > 0) {
            String username = credentials.substring(0, colonIndex);
            String password = credentials.substring(colonIndex + 1);
            
            if (username == HTTP_USERNAME && password == HTTP_PASSWORD) {
              isAuthenticated = true;
              Serial.println("Authentication successful");
            } else {
              Serial.println("Authentication failed");
            }
          }
        } else {
          Serial.println("No authentication header found");
        }

        if (!isAuthenticated) {
          // Send authentication request
          client.println("HTTP/1.1 401 Unauthorized");
          client.println("WWW-Authenticate: Basic realm=\"ESP32 Protected Area\"");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body><h1>Authentication Required</h1></body></html>");
          client.stop();
          return;
        }

        // Check if this is a toggle request
        if (requestLine.indexOf("GET /toggle") >= 0) {
          ledState = !ledState;
          digitalWrite(LED_PIN, ledState);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println();
          client.println(ledState ? "LED ON" : "LED OFF");
          client.stop();
        } else if (requestLine.indexOf("GET /events") >= 0) {
          // Accept only one SSE client at a time
          if (sseClient && sseClient.connected()) {
            sseClient.stop();
          }
          sseClient = client;
          sseActive = true;
          // Send SSE headers
          sseClient.println("HTTP/1.1 200 OK");
          sseClient.println("Content-Type: text/event-stream");
          sseClient.println("Cache-Control: no-cache");
          sseClient.println("Connection: keep-alive");
          sseClient.println("Access-Control-Allow-Origin: *");
          sseClient.println();
          // Send initial data
          float temp = temperatureRead();
          sseClient.print("data: ");
          sseClient.print("Internal Chip Temperature: ");
          sseClient.print(temp);
          sseClient.println(" &deg;C");
          sseClient.println();
          lastSSEUpdate = millis();
        } else {
          // Send the main HTML page
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("<html><head>");
          client.println("<title>ESP32 Control Panel</title>");
          client.println("<style>");
          client.println("body { font-family: Arial, sans-serif; text-align: center; margin: 50px; }");
          client.println(".button {");
          client.println("  background-color: #4CAF50;");
          client.println("  border: none;");
          client.println("  color: white;");
          client.println("  padding: 15px 32px;");
          client.println("  text-align: center;");
          client.println("  text-decoration: none;");
          client.println("  display: inline-block;");
          client.println("  font-size: 16px;");
          client.println("  margin: 4px 2px;");
          client.println("  cursor: pointer;");
          client.println("  border-radius: 4px;");
          client.println("}");
          client.println(".button:hover { background-color: #45a049; }");
          client.println("</style>");
          client.println("<script>");
          client.println("let evtSource;");
          client.println("function setupSSE() {");
          client.println("  if (evtSource) {");
          client.println("    evtSource.close();");
          client.println("  }");
          client.println("  evtSource = new EventSource('/events');");
          client.println("  evtSource.onmessage = function(event) {");
          client.println("    document.getElementById('temp').innerHTML = event.data;");
          client.println("  };");
          client.println("  evtSource.onerror = function(err) {");
          client.println("    console.error('SSE Error:', err);");
          client.println("    evtSource.close();");
          client.println("    setTimeout(setupSSE, 2000);");  // Retry after 2 seconds
          client.println("  };");
          client.println("}");
          client.println("setupSSE();");  // Initial setup
          client.println("function toggleLED() {");
          client.println("  fetch('/toggle')");
          client.println("    .then(response => response.text())");
          client.println("    .then(data => {");
          client.println("      console.log(data);");
          client.println("    })");
          client.println("    .catch(error => {");
          client.println("      console.error('Error:', error);");
          client.println("    });");
          client.println("}");
          client.println("</script>");
          client.println("</head><body>");
          client.println("<h1>ESP32 Control Panel</h1>");
          client.println("<p>Click the button to toggle the LED:</p>");
          client.println("<button class='button' onclick='toggleLED()'>Toggle LED</button>");
          client.println("<h2>Internal Temperature:</h2>");
          client.println("<div id='temp'>Loading...</div>");
          client.println("</body></html>");
          client.stop();
        }
        break;
      } else {
        if (requestLine.length() == 0) {
          requestLine = currentLine;
        } else if (currentLine.indexOf("Authorization:") >= 0) {
          authHeader = currentLine;
        }
        currentLine = "";
      }
    } else if (c != '\r') {
      currentLine += c;
    }
  }
  if (!sseActive) {
    client.stop();
  }
}

void updateSSEClient() {
  if (sseActive && sseClient && sseClient.connected()) {
    unsigned long now = millis();
    if (now - lastSSEUpdate > 1000) {
      float temp = temperatureRead();
      sseClient.print("data: ");
      sseClient.print("Internal Chip Temperature: ");
      sseClient.print(temp);
      sseClient.println(" &deg;C");
      sseClient.println();
      lastSSEUpdate = now;
    }
  } else if (sseActive) {
    // Clean up if client disconnected
    sseClient.stop();
    sseActive = false;
    Serial.println("SSE client disconnected");
  }
}

// Simple base64 decode function
String base64Decode(String input) {
  String result = "";
  int i = 0;
  int j = 0;
  unsigned char char_array_4[4], char_array_3[3];
  
  while (input.length() % 4) {
    input += "=";
  }
  
  while (i < input.length()) {
    for (int k = 0; k < 4; k++) {
      char_array_4[k] = 0;
    }
    
    for (int k = 0; k < 4; k++) {
      if (i < input.length()) {
        char_array_4[k] = base64Lookup(input[i++]);
      }
    }
    
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
    
    for (int k = 0; k < 3; k++) {
      if (char_array_4[k + 1] != 64) {
        result += (char)char_array_3[k];
      }
    }
  }
  
  return result;
}

// Base64 lookup table
unsigned char base64Lookup(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return 64;
}

void webServerTask(void *pvParameters) {
  while (true) {
    handleNewClient();
    updateSSEClient();
    vTaskDelay(1); // Yield to other tasks
  }
}

void otherTask(void *pvParameters) {
  static unsigned long lastLogTime = 0;
  while (true) {
    unsigned long currentTime = millis();
    if (currentTime - lastLogTime >= LOG_INTERVAL) {
      // Calculate uptime
      unsigned long uptime = currentTime / 1000; // Convert to seconds
      int days = uptime / 86400;
      int hours = (uptime % 86400) / 3600;
      int minutes = (uptime % 3600) / 60;
      int seconds = uptime % 60;

      Serial.println("\n--- Server Status ---");
      Serial.print("Uptime: ");
      if (days > 0) {
        Serial.print(days);
        Serial.print("d ");
      }
      Serial.print(hours);
      Serial.print("h ");
      Serial.print(minutes);
      Serial.print("m ");
      Serial.print(seconds);
      Serial.println("s");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("WiFi Status: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      Serial.println("-------------------");
      lastLogTime = currentTime;
    }
    vTaskDelay(10); // Yield to other tasks
  }
}

void loop() {
  // Not used, everything is in tasks
} 