void setup() {
  Serial.begin(115200);
}

void loop() {
  int val = analogRead(34); // Use a valid ADC-capable GPIO
  Serial.println(val);
  delay(500);
}
