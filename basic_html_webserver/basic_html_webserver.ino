#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        // Check for end of the HTTP request
        if (c == '\n' && currentLine.length() == 1) {
          // Send response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta charset='utf-8'><title>ESP32 Web Server</title></head>");
          client.println("<body><h1>Hello from ESP32!</h1>");
          client.print("<p>Uptime: ");
          client.print(millis() / 1000);
          client.println(" seconds</p>");
          client.println("</body></html>");

          client.println();
          break;
        }

        if (c == '\n') {
          currentLine = "";
        }
      }
    }

    delay(1);
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
