bool benchmarkTriggered = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ESP32 Benchmark Tool ===");
  Serial.println("Press 'Y' and Enter to start benchmark...");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'Y' || c == 'y') {
      runBenchmark();
      Serial.println("\nPress 'Y' and Enter to run benchmark again...");
    } else {
      // Optional: ignore or give feedback for other keys
      Serial.println("Invalid input. Press 'Y' to run the benchmark.");
    }

    // Clear any extra characters in buffer
    while (Serial.available()) Serial.read();
  }
}

void runBenchmark() {
  benchmarkIntegerMath();
  benchmarkFloatMath();
  benchmarkStringOps();
  benchmarkMemoryOps();
}

void benchmarkIntegerMath() {
  Serial.println("\n[ Integer Math Benchmark ]");
  const unsigned long loops = 1000000;
  volatile int result = 0;

  unsigned long start = micros();
  for (unsigned long i = 0; i < loops; ++i) {
    result += i % 7;
  }
  unsigned long duration = micros() - start;

  Serial.printf("Time: %lu µs (%lu ms)\n", duration, duration / 1000);
}

void benchmarkFloatMath() {
  Serial.println("\n[ Floating Point Math Benchmark ]");
  const unsigned long loops = 1000000;
  volatile float result = 0.0;

  unsigned long start = micros();
  for (unsigned long i = 1; i <= loops; ++i) {
    result += 1.0 / i;
  }
  unsigned long duration = micros() - start;

  Serial.printf("Time: %lu µs (%lu ms)\n", duration, duration / 1000);
}

void benchmarkStringOps() {
  Serial.println("\n[ String Operations Benchmark ]");
  const int loops = 10000;
  String s = "";

  unsigned long start = micros();
  for (int i = 0; i < loops; ++i) {
    s += "a";
  }
  unsigned long duration = micros() - start;

  Serial.printf("Final length: %d\n", s.length());
  Serial.printf("Time: %lu µs (%lu ms)\n", duration, duration / 1000);
}

void benchmarkMemoryOps() {
  Serial.println("\n[ Memory Access Benchmark ]");
  const int size = 10000;
  static int data[size];

  unsigned long start = micros();
  for (int i = 0; i < size; ++i) {
    data[i] = i * 2;
  }

  long sum = 0;
  for (int i = 0; i < size; ++i) {
    sum += data[i];
  }
  unsigned long duration = micros() - start;

  Serial.printf("Checksum: %ld\n", sum);
  Serial.printf("Time: %lu µs (%lu ms)\n", duration, duration / 1000);
}
