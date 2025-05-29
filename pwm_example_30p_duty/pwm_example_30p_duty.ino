const int pwmPin = 15;       // digital output pin 15
const int pwmFreq = 5;       // 5hz so the led blinking is visible
const int pwmResolution = 8; // 8 bit resolution (0-255)

void setup() {
  // Attach pin, frequency and resolution (channel auto assigned)
  ledcAttach(pwmPin, pwmFreq, pwmResolution);

  // Set duty cycle (0-255 for 8-bit resolution)
  ledcWrite(pwmPin, 128);  // ~50% duty cycle
}

void loop() {
  // your code here
}
