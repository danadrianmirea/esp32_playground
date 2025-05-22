
const int hallSensorPin = 25;  // Pin connected to the Hall sensor output

void setup() {
  Serial.begin(9600);             // Initialize serial communication at 9600 bps
  pinMode(hallSensorPin, INPUT);  // Set hall sensor pin as input
}

void loop() {
  int sensorValue = analogRead(hallSensorPin);  // Read analog value from Hall sensor
  Serial.print(sensorValue);                    // Output raw sensor value to Serial Monitor
  delay(200);                                   // Delay for 500 milliseconds

  // Determine magnetic pole based on sensor value
  if (sensorValue >= 2600) {
    Serial.print(" - South pole detected");  // South pole detected if value >= 2600
  } else if (sensorValue <= 1200) {
    Serial.print(" - North pole detected");  // North pole detected if value <= 1200
  }

  Serial.println();  // New line for next output
}
