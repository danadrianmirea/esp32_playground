#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WebServer server(80);

// Example: a small red dot PNG base64 (replace with your image's base64)
const char* imageBase64 = 
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQ"
  "ImWNgYGBgAAAABAABJzQnCgAAAABJRU5ErkJggg==";

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Hello from ESP32</h1>";
  html += "<img src='data:image/png;base64,";
  html += imageBase64;
  html += "'/>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
