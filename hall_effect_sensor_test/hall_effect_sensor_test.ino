// ESP32 Hall Effect Sensor Demo
// This program reads the built-in Hall effect sensor and displays the values
// The sensor can detect magnetic fields and their strength

// Define the number of samples to average for more stable readings
#define NUM_SAMPLES 10

// Variables to store sensor readings
int hallValue = 0;
int averageValue = 0;
unsigned long lastMessageTime = 0;
const unsigned long MESSAGE_INTERVAL = 5000; // 5 seconds in milliseconds

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
}

void loop() {
    // Display welcome message every 5 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastMessageTime >= MESSAGE_INTERVAL) {
        Serial.println("----------------------------------------");
        Serial.println("ESP32 Hall Effect Sensor Demo");
        Serial.println("Bring a magnet close to the ESP32 to see the values change");
        Serial.println("----------------------------------------");
        lastMessageTime = currentTime;
    }

    // Take multiple samples and average them for more stable readings
    averageValue = 0;
    for(int i = 0; i < NUM_SAMPLES; i++) {
        // Read the Hall effect sensor using the built-in function
        hallValue = analogRead(36);  // Using ADC1_CH0 (GPIO36) for Hall sensor
        averageValue += hallValue;
        delay(10); // Small delay between readings
    }
    averageValue = averageValue / NUM_SAMPLES;

    // Print the average reading
    Serial.print("Hall Effect Sensor Reading: ");
    Serial.print(averageValue);
    
    // Wait before next reading
    delay(500);
} 