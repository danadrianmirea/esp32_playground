void Task1(void *pvParameters) {
  while (true) {
    Serial.println("Task 1 is running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2(void *pvParameters) {
  while (true) {
    Serial.println("Task 2 is running");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(Task1, "Task1", 1000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task2, "Task2", 1000, NULL, 1, NULL, 1);
}

void loop() {
  // nothing here
}
