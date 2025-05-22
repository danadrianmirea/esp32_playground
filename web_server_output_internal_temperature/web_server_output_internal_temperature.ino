#include <WiFi.h>
#include "wifi_credentials.h"

#define LOG_INTERVAL 1000

// Create web server on port 80
WiFiServer server(80);

WiFiClient sseClient;
unsigned long lastSSEUpdate = 0;
bool sseActive = false;

void setup() {
  Serial.begin(115200);
  
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
}

void handleNewClient() {
  WiFiClient newClient = server.available();
  if (!newClient) return;

  String currentLine = "";
  String requestLine = "";
  unsigned long startTime = millis();

  // Wait up to 1000ms for the client to send data
  while (newClient.connected() && !newClient.available() && millis() - startTime < 1000) {
    delay(1);
  }

  while (newClient.connected() && newClient.available()) {
    char c = newClient.read();
    if (c == '\n') {
      if (currentLine.length() == 0) {
        if (requestLine.indexOf("GET /events") >= 0) {
          // Accept only one SSE client at a time
          if (sseClient && sseClient.connected()) {
            sseClient.stop();
          }
          sseClient = newClient;
          sseActive = true;
          // Send SSE headers
          sseClient.println("HTTP/1.1 200 OK");
          sseClient.println("Content-Type: text/event-stream");
          sseClient.println("Cache-Control: no-cache");
          sseClient.println("Connection: keep-alive");
          sseClient.println();
        } else {
          // Send the main HTML page
          newClient.println("HTTP/1.1 200 OK");
          newClient.println("Content-type:text/html");
          newClient.println();
          newClient.println("<html><head>");
          newClient.println("<title>ESP32 Temperature</title>");
          newClient.println("<script>");
          newClient.println("const evtSource = new EventSource('/events');");
          newClient.println("evtSource.onmessage = function(event) {");
          newClient.println("  document.getElementById('temp').innerHTML = event.data;");
          newClient.println("};");
          newClient.println("</script>");
          newClient.println("</head><body>");
          newClient.println("<h1>ESP32 Temperature Monitor</h1>");
          newClient.println("<div id='temp'>Loading...</div>");
          newClient.println("</body></html>");
          newClient.stop();
        }
        break;
      } else {
        if (requestLine.length() == 0) {
          requestLine = currentLine;
        }
        currentLine = "";
      }
    } else if (c != '\r') {
      currentLine += c;
    }
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
  }
}

void loop() {
  static unsigned long lastLogTime = 0;
  unsigned long currentTime = millis();
  
  // Log server state every LOG_INTERVAL ms
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

  handleNewClient();
  updateSSEClient();
} 