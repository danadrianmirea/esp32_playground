void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Serial Playground");
}

void loop() {
  Serial.print("Uptime (ms): ");
  Serial.println(millis());
  delay(1000);
}
