const int pwmPin = 2;              // GPIO pin connected to LED
const int pwmFreq = 3000;           // 3 kHz, no flicker
const int pwmResolution = 8;        // 8-bit resolution (0–255)
const int maxDuty = 255;            // Max value for 8-bit PWM

void setup() {
  // Setup PWM on pin 18
  ledcAttach(pwmPin, pwmFreq, pwmResolution);
}

void loop() {
  // Fade in
  for (int duty = 0; duty <= maxDuty; duty++) {
    ledcWrite(pwmPin, duty);
    delay(5);  // Adjust speed of fade
  }

  // Fade out
  for (int duty = maxDuty; duty >= 0; duty--) {
    ledcWrite(pwmPin, duty);
    delay(5);
  }
}
