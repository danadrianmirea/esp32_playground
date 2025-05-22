const int touchPin = T0; // GPIO 4 (T0)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Touch Test");
}

void loop() {
  int touchValue = touchRead(touchPin);
  Serial.print("Touch value: ");
  Serial.println(touchValue);
  delay(500);
}
