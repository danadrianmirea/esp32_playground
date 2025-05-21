#include <WiFi.h>

// Replace these with your Wi-Fi credentials
const char* ssid = "your_network";
const char* password = "your_password";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Read HTTP request but discard content
        if (c == '\n' && currentLine.length() == 0) {
          // Send HTTP response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE html><html><body><h1>Hello from ESP32!</h1></body></html>");
          break;
        } else if (c != '\r') {
          currentLine += c;
        } else {
          currentLine = "";
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
