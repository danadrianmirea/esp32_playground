void setup() {
  pinMode(2, OUTPUT); // Built-in LED on many ESP32 boards
}

void loop() {
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
}
