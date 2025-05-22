#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "";
const char* password = "";

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // List files on LittleFS (debug)
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  Serial.println("Listing files on LittleFS:");
  while (file) {
    Serial.print("  ");
    Serial.println(file.name());
    file = root.openNextFile();
  }

  // Route for root / serving index.html
  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  // Route for image
  server.on("/logo.jpg", HTTP_GET, []() {
    File file = LittleFS.open("/logo.jpg", "r");
    if (!file) {
      server.send(404, "text/plain", "Image not found");
      return;
    }
    server.streamFile(file, "image/jpeg");
    file.close();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
