#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
}

void loop() {
  Serial.println("Scanning WiFi networks...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.print(n);
    Serial.println(" networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(" dBm)");
    }
  }
  Serial.println();
  delay(10000); // Scan every 10 seconds
}
