#include "driver/dac.h"

// Define the DAC channel to use (GPIO25)
#define DAC_CHANNEL DAC_CHANNEL_1

// Sampling frequency (Hz)
#define SAMPLE_RATE 44100

// Sine wave frequency (Hz)
#define FREQUENCY 440

// Calculate the number of samples needed for one complete sine wave
#define SAMPLES_PER_CYCLE (SAMPLE_RATE / FREQUENCY)

// Buffer to store sine wave samples
uint8_t sine_wave[SAMPLES_PER_CYCLE];

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    
    // Generate sine wave samples
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        // Generate sine wave values between 0 and 255 (8-bit DAC range)
        sine_wave[i] = (sin(2 * PI * i / SAMPLES_PER_CYCLE) + 1) * 127.5;
    }
    
    // Initialize DAC
    dac_output_enable(DAC_CHANNEL);
}

void loop() {
    // Output the sine wave continuously
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        dac_output_voltage(DAC_CHANNEL, sine_wave[i]);
        // Calculate delay to maintain correct frequency
        delayMicroseconds(1000000 / SAMPLE_RATE);
    }
} 