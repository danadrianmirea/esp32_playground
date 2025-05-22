// Works with ESP32 boards
void setup() {
  Serial.begin(115200);
  delay(1000);  // Give some time for Serial to initialize
}

void loop() {
  // Get raw temperature (in °C) from the built-in sensor
  float temp = temperatureRead();

  // Print to Serial Monitor
  Serial.print("Internal Chip Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  delay(1000);  // Wait 1 second
}
