#include <WiFi.h>
#include "../libraries/ESP32_Shared_Config/ESP32_Shared_Config.h"

#define LOG_INTERVAL 1000

// Create web server on port 80
WiFiServer server(80);

TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t otherTaskHandle = NULL;

void handleNewClient();
void webServerTask(void *pvParameters);
void otherTask(void *pvParameters);

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
            }
          }
        }

        if (!isAuthenticated) {
          // Send authentication request
          client.println("HTTP/1.1 401 Unauthorized");
          client.println("WWW-Authenticate: Basic realm=\"ESP32 Protected Area\"");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body><h1>Authentication Required</h1></body></html>");
        } else {
          // Send the protected content
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("<html><head>");
          client.println("<title>ESP32 Protected Page</title>");
          client.println("</head><body>");
          client.println("<h1>Welcome to the Protected Page!</h1>");
          client.println("<p>This content is protected by Basic Authentication.</p>");
          client.println("</body></html>");
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
  client.stop();
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